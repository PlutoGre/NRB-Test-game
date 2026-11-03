"""One-time removal of unbound test guests, before the game loads player caches.

The project owner explicitly chose to discard guest saves when enabling managed
accounts. A full database backup precedes any deletion; registered UIDs survive.
"""
import json
from pathlib import Path
import re
import subprocess
import time

MARKER = 'private/account-system-v1.json'
SEED = Path(__file__).resolve().parents[1] / 'server/database/dzm.sql'


def pending(root):
    return not (root / MARKER).is_file()


def query(deployment, sql):
    result = subprocess.run([str(value) for value in deployment.database_command()] +
                            ['--batch', '--skip-column-names', '--raw', '--default-character-set=utf8', '--database=dzm'],
                            input=sql.encode('utf-8'), stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                            cwd=deployment.runtime_directory(), timeout=120, creationflags=subprocess.CREATE_NO_WINDOW)
    if result.returncode:
        log = deployment.root / 'logs/account-migration.stderr.log'
        log.write_bytes(result.stderr)
        raise RuntimeError('Guest cleanup failed; game was not started. Inspect ' + str(log))
    return result.stdout.decode('utf-8').strip()


def apply(deployment):
    if not pending(deployment.root):
        return
    if any(deployment.owned(name) for name in ('game', 'gateway', 'tunnel', 'browser', 'web_external', 'web_tunnel')):
        raise RuntimeError('Account upgrade pending: stop this deployment completely, then start it again. '
                           'Unbound test guests will be backed up and removed before managed accounts are enabled.')
    has_accounts = int(query(deployment, "SELECT COUNT(*) FROM information_schema.TABLES "
                            "WHERE TABLE_SCHEMA='dzm' AND TABLE_NAME='nrb_accounts';")) > 0
    if not has_accounts and int(query(deployment, "SELECT COUNT(*) FROM mapping WHERE LEFT(puid,5)='acct_';")):
        raise RuntimeError('Managed player IDs exist without their account table. Restore the complete database backup first.')

    def unbound(alias):
        return ('NOT EXISTS (SELECT 1 FROM nrb_accounts a WHERE a.uid=' + alias + '.puid)'
                if has_accounts else '1=1')

    players = int(query(deployment, 'SELECT COUNT(*) FROM player p WHERE ' + unbound('p') + ';'))
    mappings = int(query(deployment, 'SELECT COUNT(*) FROM mapping m WHERE ' + unbound('m') + ';'))
    backup = None
    if players or mappings:
        backup = deployment.backup()
        # Only bundled owner tables participate; custom tables and nrb account/
        # audit tables are never inferred as deletion targets from arbitrary names.
        source = SEED.read_text(encoding='utf-8-sig')
        tables = re.findall(r'CREATE TABLE `([A-Za-z0-9_]+)`\s*\((.*?)\) ENGINE=', source, re.S)
        required = {'bag', 'battle', 'career', 'disciple', 'equip', 'skill', 'skillbook', 'rewards', 'stats', 'leagua'}
        if not required.issubset({name for name, _ in tables}):
            raise RuntimeError('Bundled owner-table schema changed; guest cleanup stopped before deletion')
        sql = [
            'CREATE TEMPORARY TABLE nrb_old_guests AS SELECT p.id,p.puid FROM player p WHERE ' + unbound('p')
            + ' UNION SELECT m.gameid AS id,m.puid FROM mapping m WHERE ' + unbound('m') + ';',
            'CREATE INDEX nrb_guest_id ON nrb_old_guests(id);',
            'CREATE INDEX nrb_guest_uid ON nrb_old_guests(puid);',
            'CREATE TEMPORARY TABLE nrb_old_guilds AS SELECT l.leaguaId AS id FROM leagua l '
            'JOIN nrb_old_guests g ON l.ownerPlayerId=g.id;',
            # Index DDL can commit an existing transaction. Begin only after
            # preparing the temporary lookup tables and before the first write.
            'START TRANSACTION;',
            "UPDATE leaguamedal m JOIN nrb_old_guilds g ON m.ownerLeaguaID=g.id SET m.ownerLeaguaID=0,m.ownerLeaguaName='';",
            "UPDATE worldboss w JOIN nrb_old_guests g ON w.killerId=g.id SET w.killerId=0,w.killerName='',w.killerLevel=0,w.killerDamage=0;",
        ]
        for table, definition in tables:
            if table in {'player', 'mapping'}:
                continue
            conditions = []
            for name, kind in re.findall(r'^\s*`([A-Za-z0-9_]+)`\s+([A-Za-z]+)', definition, re.M):
                if name.lower() in {'playerid', 'ownerplayerid', 'puid', 'uid'}:
                    target = 'puid' if kind.lower() in {'varchar', 'char', 'text'} else 'id'
                    conditions.append('t.`' + name + '`=g.' + target)
            if conditions:
                sql.append('DELETE t FROM `' + table + '` t JOIN nrb_old_guests g ON ' + ' OR '.join(conditions) + ';')
        sql += [
            'DELETE p FROM player p JOIN nrb_old_guests g ON p.id=g.id;',
            'DELETE m FROM mapping m WHERE ' + unbound('m') + ';',
            'COMMIT;',
        ]
        query(deployment, '\n'.join(sql))
        print('Removed unbound test guests: players=' + str(players) + ', mappings=' + str(mappings), flush=True)
    path = deployment.root / MARKER
    temporary = path.with_suffix('.json.writing')
    temporary.write_text(json.dumps({'version': 1, 'completedAt': int(time.time()), 'removedPlayers': players,
                                     'removedMappings': mappings, 'backup': str(backup) if backup else None}, indent=2) + '\n', encoding='utf-8')
    temporary.replace(path)

