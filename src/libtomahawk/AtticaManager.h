/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2010-2011, Leo Franchi <lfranchi@kde.org>
 *
 *   Tomahawk is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Tomahawk is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Tomahawk. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ATTICAMANAGER_H
#define ATTICAMANAGER_H

#include "config.h"

#include <QObject>
#include <QHash>
#include <QPixmap>

#include "dllmacro.h"
#include "accounts/Account.h"

#include <attica/provider.h>
#include <attica/providermanager.h>
#include <attica/content.h>


class DLLEXPORT AtticaManager : public QObject
{
    Q_OBJECT
public:
    enum ResolverState {
        Uninstalled = 0,
        Installing,
        Installed,
        NeedsUpgrade,
        Upgrading,
        Failed
    };

    struct Resolver {
        QString version, scriptPath;
        int userRating; // 0-100
        ResolverState state;
        QPixmap* pixmap;

        // internal
        bool pixmapDirty;

        Resolver( const QString& v, const QString& path, int userR, ResolverState s )
            : version( v ), scriptPath( path ), userRating( userR ), state( s ), pixmap( 0 ), pixmapDirty( false ) {}
        Resolver() : userRating( -1 ), state( Uninstalled ), pixmap( 0 ), pixmapDirty( false ) {}
    };

    typedef QHash< QString, AtticaManager::Resolver > StateHash;

    static AtticaManager* instance()
    {
        if ( !s_instance )
            s_instance = new AtticaManager();

        return s_instance;
    }

    explicit AtticaManager ( QObject* parent = 0 );
    virtual ~AtticaManager();

    bool resolversLoaded() const;

    Attica::Content::List resolvers() const;
    Attica::Content resolverForId( const QString& id ) const;

    ResolverState resolverState( const Attica::Content& resolver ) const;
    QPixmap iconForResolver( const Attica::Content& id ); // Looks up in icon cache

    void uninstallResolver( const Attica::Content& resolver );
    void uninstallResolver( const QString& pathToResolver );
    QString pathFromId( const QString& resolverId ) const;

    void uploadRating( const Attica::Content& c );
    bool userHasRated( const Attica::Content& c ) const;

    /**
      If the resolver coming from libattica has a native custom c++ account
      as well. For example the last.fm account.
      */
    bool hasCustomAccountForAttica( const QString& id ) const;
    Tomahawk::Accounts::Account* customAccountForAttica( const QString& id ) const;
    void registerCustomAccount( const QString& atticaId, Tomahawk::Accounts::Account* account );

    AtticaManager::Resolver resolverData( const QString& atticaId ) const;

public slots:
    void installResolver( const Attica::Content& resolver, bool autoCreateAccount = true );
    void upgradeResolver( const Attica::Content& resolver );

signals:
    void resolversLoaded( const Attica::Content::List& resolvers );

    void resolverStateChanged( const QString& resolverId );
    void resolverInstalled( const QString& resolverId );
    void resolverUninstalled( const QString& resolverId );

private slots:
    void providerAdded( const Attica::Provider& );
    void resolversList( Attica::BaseJob* );
    void resolverDownloadFinished( Attica::BaseJob* );
    void payloadFetched();

    void loadPixmapsFromCache();
    void savePixmapsToCache();
    void resolverIconFetched();

    void syncServerData();

private:
    QString extractPayload( const QString& filename, const QString& resolverId ) const;
    void doResolverRemove( const QString& id ) const;

    Attica::ProviderManager m_manager;

    Attica::Provider m_resolverProvider;
    Attica::Content::List m_resolvers;
    StateHash m_resolverStates;

    QMap< QString, Tomahawk::Accounts::Account* > m_customAccounts;

    static AtticaManager* s_instance;
};

class DLLEXPORT CustomAtticaAccount : public Tomahawk::Accounts::Account
{
    Q_OBJECT
public:
    virtual ~CustomAtticaAccount() {}

    virtual Attica::Content atticaContent() const = 0;

protected:
    // No, you can't.
    CustomAtticaAccount( const QString& id ) : Tomahawk::Accounts::Account( id ) {}
};

Q_DECLARE_METATYPE( Attica::Content );

#endif // ATTICAMANAGER_H
