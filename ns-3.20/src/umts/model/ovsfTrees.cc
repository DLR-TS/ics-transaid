/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, CBT, EU FP7 iTETRIS project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Sendoa Vaz <svaz@cbt.es>
 */

#include <stdlib.h>
#include <stdio.h>
#include "ovsfTrees.h"
#include "ns3/ipv4-address.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("OvsfTree");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (OvsfTree);

OvsfTree::OvsfTree()
{
 
  m_left=0;
  m_right=0;
  m_next=0;
  m_up=0;
  
}

OvsfTree::OvsfTree(int maxsf)
{
  m_left=0;
  m_right=0;
  m_next=0;
  m_up=0;
  
  BuildTree(maxsf);
}

void
OvsfTree::Dispose()
{   
  m_left=0;
  m_right=0;
  m_next=0;
  m_up=0;  
}


int 
OvsfTree::check(void)
{
   
  if (root.source_ == (uint32_t)-1) {
	if (m_right != 0) {
		if ((m_right->check()) && (m_left->check()))
			return(1);
		else
			return(0);
	} else
		return(1);
  } else
  	return (0);
}


int 
OvsfTree::InsertSf(uint32_t source, int sf)
{
  int temp = 0;
  return (Insert(source, sf, temp));
}

int 
OvsfTree::Insert(uint32_t source, int sf, int temp)
{
  
  // Check temp
  
  if (root.sf_ < temp) {
	temp = root.sf_;
	if (m_next != 0) {
		return(m_next->Insert(source, sf, temp));
	} else if (m_up != 0) {
		return(m_up->Insert(source, sf, temp));
	} else {
		return(0);
	}
  } else {

	if (root.source_ != (uint32_t)-1) 
	{
		if (m_next != 0) 
		{
			return(m_next->Insert(source, sf, temp));
		} else if (m_up != 0) {
			temp = root.sf_;
			return(m_up->Insert(source, sf, temp));
		}
		else
		{
			
			return(0);
		}
	} 
	else 
	{
		if (sf == root.sf_) 
		{
			if (check()) {
				root.source_ = source;
				return(1);
			} 
			else {				
				if (m_next != 0) {
					return(m_next->Insert(source, sf, temp));
				} else if (m_up != 0) {
					temp = root.sf_;
					return(m_up->Insert(source, sf, temp));
				}else {
					
					return(0);
				}
			}
		} 
		else 
		{			
			if (m_right != 0) {
				temp = root.sf_;
				return(m_right->Insert(source, sf, temp));
			} else {
				return (0);
			}
		}
	}
  }
}

node 
OvsfTree::Search(uint32_t source, int sf, int temp)
{
  node node1;
  node1.source_=(uint32_t)-1;

  if (sf == root.sf_) {
  	if (source == root.source_) {
		return(root);
	} else {
		temp = root.sf_;
		if (m_next != 0) {
			return(m_next->Search(source, sf, temp));
		} else if (m_up != 0) {
			return(m_up->Search(source, sf, temp));
		} else {			
			return(node1);
		}
	}
  } else if (sf > root.sf_) {
	if (temp < root.sf_){
		if (m_right != 0) {
			temp = root.sf_;
			return(m_right->Search(source, sf, temp));
		}
		else
		
			return (node1);
	} else {
		temp = root.sf_;
		if (m_next != 0) {
			return(m_next->Search(source, sf, temp));
		} else if (m_up != 0) {
			return(m_up->Search(source, sf, temp));
		} else {
			return(node1);
		}
	}
  }
  else
      return(node1);

}

int 
OvsfTree::RemoveSf(uint32_t source, int sf)
{
  int temp = 0;
  
  node leaf = Search(source, sf, temp);
  if (leaf.source_ != (uint32_t)-1) {
	leaf.source_=(uint32_t)-1;
	return (1);
  }
  
  return (0);
}

void 
OvsfTree::BuildNodes(int sf, int k, int deep, int maxsf)
{
  root.source_=(uint32_t)-1;
  root.sf_=sf;
  root.k_=k;
  root.deep_=deep;
  
  if (root.sf_ < maxsf) {
    m_right = new OvsfTree();
    m_right->m_up = this;
    m_right->BuildNodes(sf*2, (2*k), (deep + 1), maxsf);
    m_left = new OvsfTree();
    m_right->m_next = m_left;
    m_left->m_up = this;
    m_left->BuildNodes(sf*2, (2*k + 1), (deep + 1), maxsf);
  }

  if ((k < sf - 1) && (sf == 4)) {
    m_next = new OvsfTree;
    m_next->BuildNodes(sf, (k + 1), deep, maxsf);
  }
  return;
}

void 
OvsfTree::BuildTree(int maxsf)
{
  BuildNodes(4, 0, 0, maxsf);
  return;
}

}//Namespace NS3

