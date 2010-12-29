#!/sbin/sh
mkdir -p /sd-ext
rm /cache/recovery/command
rm /cache/update.zip
touch /tmp/.ignorebootmessage

echo killling adbd ...


echo killing recovery
kill $(ps | grep /sbin/adbd)
kill $(ps | grep /sbin/recovery)

echo restart recovery
# On the Galaxy S, the recovery comes test signed, but the
# recovery is not automatically restarted.
/sbin/recovery &

exit 1
