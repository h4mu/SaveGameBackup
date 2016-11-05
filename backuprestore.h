#ifndef BACKUPRESTORE_H
#define BACKUPRESTORE_H

class QModelIndex;

extern bool RunBackupFor(const QModelIndex& idx);
extern bool RunRestoreFor(const QModelIndex& idx);

#endif // BACKUPRESTORE_H
