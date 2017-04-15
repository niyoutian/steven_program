/*
 * neatAVL - An avl tree algorithm implementation.
 *
 * Copyright (C) 2007 by Jaemin Kang
 *
 * This file is part of neatAVL
 *
 * neatAVL is free software; you can redistribute it and/or modify it under the 
 * terms of the GNU Lesser General Public License as published by the Free 
 * Software Foundation; either version 3 of the License, or (at your option) 
 * any later version.
 *
 * neatAVL is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for 
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author
 *  : Jaemin Kang
 *
 * Email
 *  : zaeminkr@gmail.com
 *
 * Project Page
 *  : http://sourceforge.net/projects/avltree/
 */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "AvlTreeNode.h"

class IntegerNode : public AvlTreeNode
{
   public :
      int         m_nValue;

   public :
      IntegerNode(int v) : m_nValue(v) {}
      int GetValue(){ return m_nValue; }
      int CompareTo(const Comparable* pComparable) const
      { 
          return m_nValue - (static_cast<const IntegerNode*>(pComparable))->m_nValue;
      }
};

int main(int argc, char* argv[])
{
   int                        i, j, a, b;
   time_t                     t;
   const int                  NUM_OF_TEST_NODES = 10;
   IntegerNode*               pIntegerNode[NUM_OF_TEST_NODES];
   AvlTreeNode*               pRoot;
   IntegerNode*               pTemp;
   IntegerNode*               pNode;
   IntegerNode*               pFound;
   IntegerNode**              ppReplace;
   int                        nReplace;
   AvlTreeNode::ProbePath     sProbePath;
   AvlTreeNode::TraversePath  sTraversePath;
   AvlTreeNode::probe_result  eResult;

   srand(time(&t));

AGAIN :

   /* Make 70% unique nodes.                                                  */
   for(i = 0; i < NUM_OF_TEST_NODES * 0.7; i++)
   {
      pIntegerNode[i] = new IntegerNode(i);
   }

   /* Make 30% duplicated nodes for testing.                                  */
   for(j = i; j < NUM_OF_TEST_NODES; j++)
   {
      pIntegerNode[j] = new IntegerNode(abs(rand()) % i);
   }

   /* shuffle them enough.                                                    */
   for(i = 0; i < NUM_OF_TEST_NODES * 2; i++)
   {
      a = rand() % NUM_OF_TEST_NODES;
      b = rand() % NUM_OF_TEST_NODES;

      pTemp             = pIntegerNode[a];
      pIntegerNode[a]   = pIntegerNode[b];
      pIntegerNode[b]   = pTemp;
   }

   /* Set the root.                                                           */
   pRoot = pIntegerNode[0];
   printf("Root : %d\n", pIntegerNode[0]->GetValue());

   /* Insert 50% of them into the tree by calling the first insertion function*/
   for(i = 1; i < NUM_OF_TEST_NODES * 0.5; i++)
   {
      printf("Insert : %d\n", pIntegerNode[i]->GetValue());

      /* Insert a node. When a duplicated node is inserted, it will be        */
      /* rejected naturally and there is no way to notice that. If you really */
      /* need to be sure the result of insertion, use the other insertion     */
      /* function.                                                            */
      pRoot = pRoot->Insert(pIntegerNode[i]);
   }

   /* Insert left of them into the tree by calling the second insertion       */
   /* function.                                                               */
   for( ; i < NUM_OF_TEST_NODES; i++)
   {
      printf("Probe & Insert: %d", pIntegerNode[i]->GetValue());

      /* Probe first.                                                         */
      eResult = pRoot->Probe(pIntegerNode[i], &sProbePath);

      switch(eResult)
      {
         case AvlTreeNode::FOUND     : printf(" -> found.\n"); break;
         case AvlTreeNode::NOT_FOUND : printf(" -> not found.\n"); break;
         case AvlTreeNode::FULL      : printf(" -> the tree is full.\n"); break;
      }

      /* We just push the node into the tree for testing no matter what       */
      /* result we got. If 'bResult == false', the insertion will be rejected */
      /* naturally.                                                           */
      pRoot = pRoot->Insert(&sProbePath, pIntegerNode[i]);
   }

   /* Find and replace some of them for testing.                              */
   nReplace = static_cast<int>(NUM_OF_TEST_NODES * 0.2);
   ppReplace = new IntegerNode*[nReplace];
   for(i = 0; i < nReplace; i++)
   {
      ppReplace[i] = new IntegerNode(abs(rand()) % NUM_OF_TEST_NODES);

      /* Probe first.                                                         */
      printf("Probe : %d", ppReplace[i]->GetValue());
      eResult = pRoot->Probe(ppReplace[i], &sProbePath);

      switch(eResult)
      {
         case AvlTreeNode::NOT_FOUND :
            printf(" -> not found.\n");

            /* Although it was not found, just access the node for testing.      */
            pFound = static_cast<IntegerNode*>(sProbePath.Access());
            printf("Found : 0x%08X\n", pFound); /* Must be null.                   */
            continue;
            break;

         case AvlTreeNode::FOUND :
            printf(" -> found.\n");

            /* Access the node.                                                  */
            pFound = static_cast<IntegerNode*>(sProbePath.Access());
            printf("Found : %d\n", pFound->GetValue());

            /* Replace the node.                                                 */
            printf("Replace : %d\n", pFound->GetValue());
            pNode = ppReplace[i];
            pRoot = pRoot->Replace(&sProbePath, reinterpret_cast<AvlTreeNode**>(&pNode));
            break;

         case AvlTreeNode::FULL :
            printf(" -> the tree is full.\n");
            break;
      }
   }

   /* Traverse nodes.                                                         */
   printf("Traverse : ");
   pRoot->InitTraverse(&sTraversePath);

   pNode = static_cast<IntegerNode*>(pRoot->Traverse(&sTraversePath));

   do
   {
      printf("%d ", pNode->GetValue());
      pNode = static_cast<IntegerNode*>(pRoot->Traverse(&sTraversePath));
   }while(pNode);

   printf("\n");

   /* Get the smallest node.                                                  */
   pNode = static_cast<IntegerNode*>(pRoot->GetMinNode());
   printf("Min Node : %d\n", pNode->GetValue());

   /* Get the largest node.                                                   */
   pNode = static_cast<IntegerNode*>(pRoot->GetMaxNode());
   printf("Max Node : %d\n", pNode->GetValue());

   /* Remove 50% nodes by calling the first remove function.                  */
   for(i = 0; i < NUM_OF_TEST_NODES * 0.5; i++)
   {
      printf("Remove : %d\n", pIntegerNode[i]->GetValue());
      pNode = pIntegerNode[i];
      pRoot = pRoot->Remove(pIntegerNode[i], reinterpret_cast<AvlTreeNode**>(&pNode));
   }

   /* Remove the left nodes by calling the second remove function.            */
   for( ; i < NUM_OF_TEST_NODES; i++)
   {
      /* Probe first.                                                         */
      printf("Probe & Remove : %d", pIntegerNode[i]->GetValue());
      eResult = pRoot->Probe(pIntegerNode[i], &sProbePath);

      switch(eResult)
      {
         case AvlTreeNode::FOUND     : printf(" -> found.\n"); break;
         case AvlTreeNode::NOT_FOUND : printf(" -> not found.\n"); break;
         case AvlTreeNode::FULL      : printf(" -> the tree is full.\n"); break;
      }

      /* We just try to remove the node from the tree for testing no matter   */
      /* what result we got. If 'bResult == false', the removal will be       */
      /* rejected naturally.                                                  */
      pNode = pIntegerNode[i];
      pRoot = pRoot->Remove(&sProbePath, reinterpret_cast<AvlTreeNode**>(&pNode));
   }

   for(i = 0; i < NUM_OF_TEST_NODES; i++)
   {
      delete pIntegerNode[i];
   }

   for(i = 0; i < nReplace; i++)
   {
      delete ppReplace[i];
   }

   goto AGAIN;

   return 0;
}


