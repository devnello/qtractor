// qtractorCurveCommand.cpp
//
/****************************************************************************
   Copyright (C) 2005-2011, rncbc aka Rui Nuno Capela. All rights reserved.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*****************************************************************************/

#include "qtractorAbout.h"
#include "qtractorCurveCommand.h"


//----------------------------------------------------------------------
// class qtractorCurveCommand - declaration.
//

// Constructor.
qtractorCurveCommand::qtractorCurveCommand ( const QString& sName )
	: qtractorCommand(sName)
{
}

// Destructor.
qtractorCurveCommand::~qtractorCurveCommand (void)
{
	QListIterator<Item *> iter(m_items);
	while (iter.hasNext()) {
		Item *pItem = iter.next();
		if (pItem->autoDelete)
			delete pItem->node;
	}

	qDeleteAll(m_items);
	m_items.clear();
}


// Primitive command methods.
void qtractorCurveCommand::addNode (
	qtractorCurve *pCurve, qtractorCurve::Node *pNode )
{
	m_items.append(new Item(AddNode, pCurve, pNode));
}


void qtractorCurveCommand::removeNode (
	qtractorCurve *pCurve, qtractorCurve::Node *pNode )
{
	m_items.append(new Item(RemoveNode, pCurve, pNode));
}


// Composite predicate.
bool qtractorCurveCommand::isEmpty (void) const
{
	return m_items.isEmpty();
}


// Common executive method.
bool qtractorCurveCommand::execute ( bool bRedo )
{
	QListIterator<Item *> iter(m_items);
	if (!bRedo)
		iter.toBack();
	while (bRedo ? iter.hasNext() : iter.hasPrevious()) {
		Item *pItem = (bRedo ? iter.next() : iter.previous());
		qtractorCurve *pCurve = pItem->curve;
		// Execute the command item...
		switch (pItem->command) {
		case AddNode:
			if (bRedo)
				pCurve->insertNode(pItem->node);
			else
				pCurve->unlinkNode(pItem->node);
			break;
			pItem->autoDelete = !bRedo;
		case RemoveNode:
			if (bRedo)
				pCurve->unlinkNode(pItem->node);
			else
				pCurve->insertNode(pItem->node);
			pItem->autoDelete = bRedo;
			break;
		default:
			break;
		}
	}

	return true;
}


// Virtual command methods.
bool qtractorCurveCommand::redo (void)
{
	return execute(true);
}

bool qtractorCurveCommand::undo (void)
{
	return execute(false);
}


// end of qtractorCurveCommand.cpp
