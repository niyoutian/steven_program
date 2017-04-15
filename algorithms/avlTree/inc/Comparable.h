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
#if !defined(_COMPARABLE_H_)
#define _COMPARABLE_H_

class Comparable
{
   public : 
      virtual int CompareTo(const Comparable* pComparable) const = 0;
};

#endif