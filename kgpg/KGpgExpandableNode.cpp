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
#include "KGpgExpandableNode.h"

//#include "kgpgsettings.h"
#include "convert.h"
//#include "model/kgpgitemmodel.h"

//#include <KLocale>

KGpgExpandableNode::KGpgExpandableNode(KGpgExpandableNode *parent)
	: KGpgNode(parent)
{
	if (parent != NULL)
		parent->children.append(this);
}

KGpgExpandableNode::~KGpgExpandableNode()
{
	for (int i = children.count() - 1; i >= 0; i--)
		delete children[i];
}

KGpgNode *
KGpgExpandableNode::getChild(const int index) const
{
	if ((index < 0) || (index > children.count()))
		return NULL;
	return children.at(index);
}

int
KGpgExpandableNode::getChildCount()
{
	if (children.count() == 0)
		readChildren();

	return children.count();
}

bool
KGpgExpandableNode::hasChildren() const
{
	return (children.count() != 0);
}

bool
KGpgExpandableNode::wasExpanded() const
{
	return (children.count() != 0);
}

const
KGpgNode::List &
KGpgExpandableNode::getChildren() const
{
	return children;
}

int
KGpgExpandableNode::getChildIndex(KGpgNode *node) const
{
	return children.indexOf(node);
}

void
KGpgExpandableNode::deleteChild(KGpgNode *child)
{
	children.removeAll(child);
}

//#include "KGpgExpandableNode.moc"
