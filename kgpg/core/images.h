/***************************************************************************
 *   Copyright (C) 2006 by Jimmy Gilles                                    *
 *   jimmygilles@gmail.com                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#ifndef IMAGES_H
#define IMAGES_H

#include <QPixmap>

namespace KgpgCore
{

class Images
{
public:
    static QPixmap single();
    static QPixmap pair();
    static QPixmap group();
    static QPixmap orphan();
    static QPixmap signature();
    static QPixmap userId();
    static QPixmap photo();
    static QPixmap revoke();

    /* Desktop image */
    static QPixmap kgpg();
};

} // namespace KgpgCore

#endif // IMAGES_H
