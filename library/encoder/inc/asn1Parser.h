#ifndef __ASN1_PARSER_H__
#define __ASN1_PARSER_H__
#include "mxDef.h"

#if 1
/**
 * Definition of some primitive ASN1 types
 */
typedef enum {
	ASN1_PARSER_EOC =				0x00,
	ASN1_PARSER_BOOLEAN =			0x01,
	ASN1_PARSER_INTEGER =			0x02,
	ASN1_PARSER_BIT_STRING =		0x03,
	ASN1_PARSER_OCTET_STRING =		0x04,
	ASN1_PARSER_NULL =				0x05,
	ASN1_PARSER_OID =				0x06,
	ASN1_PARSER_ENUMERATED =		0x0A,
	ASN1_PARSER_UTF8STRING =		0x0C,
	ASN1_PARSER_NUMERICSTRING =	    0x12,
	ASN1_PARSER_PRINTABLESTRING =	0x13,
	ASN1_PARSER_T61STRING =		    0x14,
	ASN1_PARSER_VIDEOTEXSTRING =	0x15,
	ASN1_PARSER_IA5STRING =		    0x16,
	ASN1_PARSER_UTCTIME =			0x17,
	ASN1_PARSER_GENERALIZEDTIME =	0x18,
	ASN1_PARSER_GRAPHICSTRING =	    0x19,
	ASN1_PARSER_VISIBLESTRING =	    0x1A,
	ASN1_PARSER_GENERALSTRING =	    0x1B,
	ASN1_PARSER_UNIVERSALSTRING =	0x1C,
	ASN1_PARSER_BMPSTRING =		    0x1E,

	ASN1_PARSER_CONSTRUCTED =		0x20,

	ASN1_PARSER_SEQUENCE =			0x30,
	ASN1_PARSER_SET =				0x31,

	ASN1_PARSER_CONTEXT_S_0 =		0x80,
	ASN1_PARSER_CONTEXT_S_1 =		0x81,
	ASN1_PARSER_CONTEXT_S_2 =		0x82,
	ASN1_PARSER_CONTEXT_S_3 =		0x83,
	ASN1_PARSER_CONTEXT_S_4 =		0x84,
	ASN1_PARSER_CONTEXT_S_5 =		0x85,
	ASN1_PARSER_CONTEXT_S_6 =		0x86,
	ASN1_PARSER_CONTEXT_S_7 =		0x87,
	ASN1_PARSER_CONTEXT_S_8 =		0x88,

	ASN1_PARSER_CONTEXT_C_0 =		0xA0,
	ASN1_PARSER_CONTEXT_C_1 =		0xA1,
	ASN1_PARSER_CONTEXT_C_2 =		0xA2,
	ASN1_PARSER_CONTEXT_C_3 =		0xA3,
	ASN1_PARSER_CONTEXT_C_4 =		0xA4,
	ASN1_PARSER_CONTEXT_C_5 =		0xA5,

	ASN1_PARSER_INVALID =			0x100,

} asn1_t;
#endif

/**
 * Definition of ASN.1 flags
 */
#define ASN1_NONE    0x0000
#define ASN1_DEF     0x0001
#define ASN1_OPT     0x0002
#define ASN1_LOOP    0x0004
#define ASN1_CHOICE  0x0008
#define ASN1_CH      0x0010
#define ASN1_END     0x0020
#define ASN1_OBJ     0x0040
#define ASN1_BODY    0x0080
#define ASN1_RAW     0x0100
#define ASN1_EXIT    0x0200


#define ASN1_MAX_LEVEL	10
#define ASN1_INVALID_LENGTH	0xffffffff


typedef struct asn1Object_t asn1Object_t;

/**
 * Syntax definition of an ASN.1 object
 */
struct asn1Object_t{
	u32_t level;
	const s8_t *name;
	asn1_t type;
	u16_t flags;
};


class asn1Parser
{
public:
	asn1Parser(asn1Object_t const *objects, chunk_t blob);
	~asn1Parser();
	bool iterate(s32_t& objectID, chunk_t& object);
	void setFlags(bool implicit, bool privated);
	bool isAsn1Encode(chunk_t blob);
private:
	u32_t asn1Length(chunk_t *blob);
	
	asn1Object_t const *mpObjects;  //Ä£°æ

	s32_t mLine;   //Current syntax definition line
	bool mSuccess; // Current stat of the parsing operation
	bool mPrivate; //Declare object data as private - use debug level 4 to log it
	bool mImplicit; //Top-most type is implicit - ignore it
	s32_t mLevel0; //Top-most parsing level - defaults to 0

	s32_t   mLoopAddr[ASN1_MAX_LEVEL + 1]; //Jump back address for loops for each level
	chunk_t mBlobs[ASN1_MAX_LEVEL + 2]; //Current parsing pointer for each level
	bool mChoice[ASN1_MAX_LEVEL + 2];  //Parsing a CHOICE on the current level ?

};


#endif

