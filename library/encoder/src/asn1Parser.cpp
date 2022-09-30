#include <string.h>
#include "mxLog.h"
#include "asn1Parser.h"




/**
 * Create an ASN.1 parser
 *
 * @param objects	syntax definition of the ASN.1 object to be parsed
 * @param blob		ASN.1 coded binary blob
 */
asn1Parser::asn1Parser(asn1Object_t const *objects, chunk_t blob)
{
	mLine = -1;
	mLevel0 = 0;
	mImplicit = false;
	mPrivate = false;
	mSuccess = true;
	memset(mLoopAddr,0,sizeof(mLoopAddr));
	memset(mBlobs,0,sizeof(mBlobs));
	memset(mChoice,0,sizeof(mChoice));
	mpObjects = objects;
	mBlobs[0] = blob;
}

asn1Parser::~asn1Parser()
{
	
}

bool asn1Parser::iterate(s32_t& objectID, chunk_t& object)
{
	u32_t level;
	chunk_t *blob = NULL, *blob1 = NULL, blob_ori;
	asn1Object_t obj;
	u8_t *start_ptr = NULL;

	object.ptr = NULL;
	object.len = 0;
	/* Advance to the next object syntax definition line */
	mLine++;
	obj = mpObjects[mLine];
	
	/* Terminate if the end of the object syntax definition has been reached */
	if (obj.flags & ASN1_EXIT) {
		return false;
	}
	/* end of loop or choice or option found */
	if (obj.flags & ASN1_END){
		if (mLoopAddr[obj.level] && this->mBlobs[obj.level+1].len > 0) {
			mLine = mLoopAddr[obj.level]; /* another iteration */
			obj = mpObjects[mLine];
		}else{
			mLoopAddr[obj.level] = 0;		 /* exit loop */
			if (obj.flags & ASN1_CHOICE) /* end of choices */
			{
				if (mChoice[obj.level+1]){
					//DBG1(DBG_ASN, "L%d - %s:  incorrect choice encoding", mLevel0 + obj.level, obj.name);
					mSuccess = false;
					goto end;
				}
			}

			if (obj.flags & ASN1_CH) /* end of choice */
			{
				/* parsed a valid choice */
				mChoice[obj.level] = false;
				/* advance to end of choices */
				do
				{
					mLine++;
				}
				while (!((mpObjects[mLine].flags & ASN1_END) &&
						 (mpObjects[mLine].flags & ASN1_CHOICE) &&
						 (mpObjects[mLine].level == obj.level-1)));
				mLine--;
			}

			goto end;
		}
	}
	
	level = mLevel0 + obj.level;
	blob = mBlobs + obj.level;
	blob_ori = *blob;
	blob1 = blob + 1;
	start_ptr = blob->ptr;

	/* handle ASN.1 defaults values */
	if ((obj.flags & ASN1_DEF) && (blob->len == 0 || *start_ptr != obj.type) )
	{
		/* field is missing */
		if (obj.type & ASN1_PARSER_CONSTRUCTED){
			mLine++ ;  /* skip context-specific tag */
		}
		goto end;
	}
	/* handle ASN.1 options */
	if ((obj.flags & ASN1_OPT)
			&& (blob->len == 0 || *start_ptr != obj.type)) {
		/* advance to end of missing option field */
		do
		{
			mLine++;
		}
		while (!((mpObjects[mLine].flags & ASN1_END) &&
				 (mpObjects[mLine].level == obj.level)));
		goto end;
	}


	/* an ASN.1 object must possess at least a tag and length field */
	if (blob->len < 2) {
		mSuccess = false;
		goto end;
	}
	blob1->len = asn1Length(blob);

	if (blob1->len == ASN1_INVALID_LENGTH) {
		//mxLogFmt(LOG_DEBUG,"L%d - %s:  length of ASN.1 object invalid or too large\n",level, obj.name);
		mSuccess = false;
		goto end;
	}
	blob1->ptr = blob->ptr;
	blob->ptr += blob1->len;
	blob->len -= blob1->len;

	/* handle ASN.1 choice without explicit context encoding */
	if ((obj.flags & ASN1_CHOICE) && obj.type == ASN1_PARSER_EOC){
		mChoice[obj.level+1] = true;
		*blob1 = blob_ori;
		goto end;
	}

	/* return raw ASN.1 object without prior type checking */
	if (obj.flags & ASN1_RAW){
		object.ptr = start_ptr;
		object.len = (u32_t)(blob->ptr - start_ptr);
		goto end;
	}

	if (*start_ptr != obj.type && !(mImplicit && mLine == 0))
	{
		mSuccess = false;
		goto end;
	}
	//mxLogFmt(LOG_DEBUG,"L%d - %s:\n",level, obj.name);

	/* In case of "SEQUENCE OF" or "SET OF" start a loop */
	if (obj.flags & ASN1_LOOP){
		if (blob1->len > 0){
			/* at least one item, start the loop */
			mLoopAddr[obj.level] = mLine + 1;
		}
		else
		{
			/* no items, advance directly to end of loop */
			do
			{
				mLine++;
			}
			while (!((mpObjects[mLine].flags & ASN1_END) &&
					 (mpObjects[mLine].level == obj.level)));
			goto end;
		}
	}

	/* In case of a "CHOICE" start to scan for exactly one valid choice */
	if (obj.flags & ASN1_CHOICE){
		if (blob1->len == 0){
			//mxLogFmt(LOG_DEBUG,"L%d - %s:contains no choice\n",level, obj.name);
			mSuccess = false;
			goto end;
		}
		mChoice[obj.level+1] = true;
	}

	if (obj.flags & ASN1_OBJ){
		object.ptr = start_ptr;
		object.len = (u32_t)(blob->ptr - start_ptr);
	}
	else if (obj.flags & ASN1_BODY) {
		object = *blob1;
	}


end:
	objectID = mLine;
	return mSuccess;

}

u32_t asn1Parser::asn1Length(chunk_t *blob)
{
	u8_t n;
	s32_t len;

	if (blob->len < 2){
		return ASN1_INVALID_LENGTH;
	}

	/* read length field, skip tag and length */
	n = blob->ptr[1];
	blob->ptr += 2;
	blob->len -= 2;

	if ((n & 0x80) == 0)
	{	/* single length octet */
		if (n > blob->len){
			mxLogFmt(LOG_DEBUG,"length is larger than remaining blob size\n");
			return ASN1_INVALID_LENGTH;
		}
		return n;
	}

	/* composite length, determine number of length octets */
	n &= 0x7f;

	if (n == 0 || n > blob->len){
		mxLogFmt(LOG_DEBUG,"number of length octets invalid\n");
		return ASN1_INVALID_LENGTH;
	}

	if (n > sizeof(len)){
		return ASN1_INVALID_LENGTH;
	}

	len = 0;

	while (n-- > 0){
		len = 256*len + *blob->ptr++;
		blob->len--;
	}
	if (len > blob->len){
		mxLogFmt(LOG_DEBUG,"length is larger than remaining blob size\n");
		return ASN1_INVALID_LENGTH;
	}
	return len;
}

void asn1Parser::setFlags(bool implicit, bool privated)
{
	mImplicit = implicit;
	mPrivate = privated;
}

bool asn1Parser::isAsn1Encode(chunk_t blob)
{
	if (!blob.len || !blob.ptr){
		return false;
	}

	u8_t tag = 0;
	tag = *blob.ptr;
	if (tag != ASN1_PARSER_SEQUENCE && tag != ASN1_PARSER_SET && tag != ASN1_PARSER_OCTET_STRING) {
		return false;
	}

	u32_t len = 0;
	len = asn1Length(&blob);
	if (len == ASN1_INVALID_LENGTH) {
		return false;
	}
	/* exact match */
	if (len == blob.len){
		return true;
	}

	/* some websites append a surplus newline character to the blob */
	if (len + 1 == blob.len && *(blob.ptr + len) == '\n'){
		return true;
	}

	mxLogFmt(LOG_DEBUG,"file size does not match ASN.1 coded length\n");
	return false;
}

