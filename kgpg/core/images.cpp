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

#include "images.h"

//#include <KIconLoader>

namespace KgpgCore
{

QPixmap Images::single()
{
    static QPixmap single;
//    if (single.isNull())
//        single = KIconLoader::global()->loadIcon(QLatin1String( "key-single" ), KIconLoader::Small, 20);
    return single;
}

QPixmap Images::pair()
{
    static QPixmap pair;
//    if (pair.isNull())
//        pair = KIconLoader::global()->loadIcon(QLatin1String( "key-pair" ), KIconLoader::Small, 20);
    return pair;
}

QPixmap Images::group()
{
    static QPixmap group;
//    if (group.isNull())
//        group = KIconLoader::global()->loadIcon(QLatin1String( "key-group" ), KIconLoader::Small, 20);
    return group;
}

QPixmap Images::orphan()
{
    static QPixmap oprpan;
//    if (oprpan.isNull())
//        oprpan = KIconLoader::global()->loadIcon(QLatin1String( "key-orphan" ), KIconLoader::Small, 20);
    return oprpan;
}

QPixmap Images::signature()
{
    static QPixmap signature;
//    if (signature.isNull())
//        signature = KIconLoader::global()->loadIcon(QLatin1String( "application-pgp-signature" ), KIconLoader::Small, 20);
    return signature;
}

QPixmap Images::userId()
{
    static QPixmap userid;
//    if (userid.isNull())
//        userid = KIconLoader::global()->loadIcon(QLatin1String( "x-office-contact" ), KIconLoader::Small, 20);
    return userid;
}

QPixmap Images::photo()
{
    static QPixmap photo;
//    if (photo.isNull())
//        photo = KIconLoader::global()->loadIcon(QLatin1String( "image-x-generic" ), KIconLoader::Small, 20);
    return photo;
}

QPixmap Images::revoke()
{
    static QPixmap revoke;
//    if (revoke.isNull())
//        revoke = KIconLoader::global()->loadIcon(QLatin1String( "dialog-error" ), KIconLoader::Small, 20);
    return revoke;
}

QPixmap Images::kgpg()
{
    static QPixmap kgpg;
//    if (kgpg.isNull())
//        kgpg = KIconLoader::global()->loadIcon(QLatin1String( "kgpg" ), KIconLoader::Desktop);
    return kgpg;
}

} // namespace KgpgCore
