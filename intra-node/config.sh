oc new-project ipc
oc adm policy add-scc-to-user privileged -z default -n ipc



# logs and process ids
ps -ef | grep ipc-shared-memory | grep -v grep | awk '{print $2, $16}'
ps -ef | grep hostipc-shared-memory | grep -v grep | awk '{print $2, $16}'


cat /proc/343093/maps
ls -lh /proc/343093/root/dev/shm