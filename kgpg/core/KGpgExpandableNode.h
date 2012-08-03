/* Copyright 2008,2009,2012 Rolf Eike Beer <kde@opensource.sf-tec.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef KGPGEXPANDABLENODE_H
#define KGPGEXPANDABLENODE_H

#include "KGpgNode.h"

class KGpgSubkeyNode;
class KGpgRefNode;

/**
 * @brief The abstract base class for all classes that may have child objects
 *
 * Every class that represents something in the keyring that may have
 * child objects inherits from this class. That does not mean that every
 * child object always has children, but every child \em may have children.
 */
class KGpgExpandableNode : public KGpgNode
{
	Q_OBJECT

	friend class KGpgRefNode;
	friend class KGpgSubkeyNode;
protected:
	KGpgNode::List children;

	/**
	 * reimplemented in every base class to read in the child data
	 *
	 * This allows the child objects to delay the loading of the
	 * child objects until they are really needed to avoid time
	 * consuming operations for data never used.
	 */
	virtual void readChildren() = 0;

	explicit KGpgExpandableNode(KGpgExpandableNode *parent = NULL);
public:
	virtual ~KGpgExpandableNode();

	/**
	 * check if there are any child nodes
	 *
	 * The default implementation returns true if any child nodes were loaded.
	 * This may be reimplemented by child classes so they can indicate that
	 * there are child nodes before actually loading them.
	 *
	 * This method indicates if there are children if this node is expanded.
	 * In contrast wasExpanded() will only return true if the child nodes
	 * are actually present in memory.
	 */
	virtual bool hasChildren() const;
	/**
	 * check if there are any child nodes present in memory
	 *
	 * Returns true if any child nodes were loaded.
	 *
	 * This method indicates if the children of this node are already loaded
	 * into memory. In contrast hasChildren() may return true even if the child
	 * objects are not present in memory.
	 */
	virtual bool wasExpanded() const;
	virtual int getChildCount();
	virtual const KGpgNode::List &getChildren() const;
	virtual KGpgNode *getChild(const int index) const;
	virtual int getChildIndex(KGpgNode *node) const;
	virtual void deleteChild(KGpgNode *child);
};

#endif /* KGPGEXPANDABLENODE_H */
