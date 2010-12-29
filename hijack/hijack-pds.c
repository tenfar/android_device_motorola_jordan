#include "hijack.h"

int exec_and_wait(char ** argp)
{
    pid_t pid;
    sig_t intsave, quitsave;
    sigset_t mask, omask;
    int pstat;

    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &mask, &omask);
    switch (pid = vfork()) {
    case -1:            /* error */
        sigprocmask(SIG_SETMASK, &omask, NULL);
        return(-1);
    case 0:                /* child */
        sigprocmask(SIG_SETMASK, &omask, NULL);
        execve(argp[0], argp, environ);
    _exit(127);
  }

    intsave = (sig_t)  bsd_signal(SIGINT, SIG_IGN);
    quitsave = (sig_t) bsd_signal(SIGQUIT, SIG_IGN);
    pid = waitpid(pid, (int *)&pstat, 0);
    sigprocmask(SIG_SETMASK, &omask, NULL);
    (void)bsd_signal(SIGINT, intsave);
    (void)bsd_signal(SIGQUIT, quitsave);
    return (pid == -1 ? -1 : pstat);
}

int remount_root(const char * hijack_exec, int rw) {
    char * remount_root_args[5];
    remount_root_args[0] = strdup(hijack_exec);
    remount_root_args[1] = strdup("mount");
    if (rw > 0) {
        remount_root_args[2] = strdup("-orw,remount");
    } else {
        remount_root_args[2] = strdup("-oro,remount");
    }
    remount_root_args[3] = strdup("/");
    remount_root_args[4] = NULL;
    return exec_and_wait(remount_root_args);
}

int hijack_mount(const char * hijack_exec, const char * dev, const char * mount_point) {
    char * mount_args[5];
    mount_args[0] = strdup(hijack_exec);
    mount_args[1] = strdup("mount");
    mount_args[2] = strdup(dev);
    mount_args[3] = strdup(mount_point);
    mount_args[4] = NULL;
    return exec_and_wait(mount_args);
}

int hijack_mount_ex(const char * hijack_exec, const char * type, const char * dev, const char * mount_point) {
    char * mount_args[7];
    mount_args[0] = strdup(hijack_exec);
    mount_args[1] = strdup("mount");
    mount_args[2] = strdup("-t");
    mount_args[3] = strdup(type);
    mount_args[4] = strdup(dev);
    mount_args[5] = strdup(mount_point);
    mount_args[6] = NULL;
    return exec_and_wait(mount_args);
}

int hijack_umount(const char * hijack_exec, const char * mount_point) {
    char * umount_args[5];
    umount_args[0] = strdup(hijack_exec);
    umount_args[1] = strdup("umount");
    umount_args[2] = strdup("-l");
    umount_args[3] = strdup(mount_point);
    umount_args[4] = NULL;
    return exec_and_wait(umount_args);
}

void hijack_log(char * format, ...) {
#ifdef LOG_ENABLE
    FILE * log = fopen(LOG_PATH, "a");
    if (log != NULL) {
        // start our va list
        va_list args;
        va_start(args, format);

        // some buffers :D
        char * new_format = (char *)malloc(sizeof(char) * PATH_MAX);
        char * time_buffer = (char *)malloc(sizeof(char) * PATH_MAX);
        char * out_buffer = (char *)malloc(sizeof(char) * PATH_MAX);

        // get the time
        time_t * ts = (time_t *)malloc(sizeof(time_t));
        time(ts);
        struct tm * t = localtime(ts);
        strftime(time_buffer, PATH_MAX, "%Y-%m-%d %H:%M:%S", t);
        free(ts);

        sprintf(new_format, "hijack [%s]: %s\n", time_buffer, format);
        vsprintf(out_buffer, new_format, args);
        fputs(out_buffer, log);
        va_end(args);
        fclose(log);

        free(new_format);
        free(time_buffer);
        free(out_buffer);
    }
#endif
}

int mark_file(char * filename) {
    FILE *f = fopen(filename, "wb");
    fwrite("1", 1, 1, f);
    if (f != NULL) {
        fclose(f);
        return 0;
    } else {
        return -1;
    }
}

int main(int argc, char ** argv) {
    char * hijacked_executable = argv[0];
    struct stat info;
    int result = 0;
    int i;

    if (NULL != strstr(hijacked_executable, "hijack")) {
        // no op
        if (argc >= 2) {
            return busybox_driver(argc - 1, argv + 1);
        }

        return 0;
    }



    // check to see if hijack was already run, and if so, just continue on.
    if (argc >= 3 && 0 == strcmp(argv[2], "preinstall")) {

hijack_log("  Entering testing for hijacking!");

            if (0 == stat(RECOVERY_MODE_FILE, &info)) {

hijack_log("  Recovery mode detected!");

                // don't boot into recovery again
hijack_log("    remove(%s) executing...", RECOVERY_MODE_FILE);
                result = remove(RECOVERY_MODE_FILE);
hijack_log("      returned: %d", result);

hijack_log("    creating(%s) executing...", HIJACK_BYPASS_FILE);
                result = mark_file(HIJACK_BYPASS_FILE);
hijack_log("      returned: %d", result);


hijack_log("    remount_root(%s, %d) executing...", "/system/bin/hijack", 1);
                result = remount_root("/system/bin/hijack", 1);
hijack_log("      returned: %d", result);

hijack_log("    mkdir(%s, %d) executing...", "/pds", S_IRWXU);
                result = mkdir("/pds", S_IRWXU);
hijack_log("      returned: %d", result);

hijack_log("    mkdir(%s, %d) executing...", "/tmp", S_IRWXU);
                result = mkdir("/tmp", S_IRWXU);
hijack_log("      returned: %d", result);

hijack_log("    mkdir(%s, %d) executing...", "/res", S_IRWXU);
                result = mkdir("/res", S_IRWXU);
hijack_log("      returned: %d", result);

hijack_log("    mkdir(%s, %d) executing...", "/res/images", S_IRWXU);
                result = mkdir("/res/images", S_IRWXU);
hijack_log("      returned: %d", result);

hijack_log("    remove(%s) executing...", "/etc");
                result = remove("/etc");
hijack_log("      returned: %d", result);

hijack_log("    mkdir(%s, %d) executing...", "/etc", S_IRWXU);
                result = mkdir("/etc", S_IRWXU);
hijack_log("      returned: %d", result);

hijack_log("    rename(%s, %s) executing...", "/sbin/adbd", "/sbin/adbd.old");
                result = rename("/sbin/adbd", "/sbin/adbd.old");
hijack_log("      returned: %d", result);


hijack_log("    property_set(%s, %s) executing...", "ctl.stop", "runtime");
                result = property_set("ctl.stop", "runtime");
hijack_log("      returned: %d", result);

hijack_log("    property_set(%s, %s) executing...", "ctl.stop", "zygote");
                result = property_set("ctl.stop", "zygote");
hijack_log("      returned: %d", result);

hijack_log("    property_set(%s, %s) executing...", "persist.service.adb.enable", "1");
                result = property_set("persist.service.adb.enable", "1");
hijack_log("      returned: %d", result);

hijack_log("    hijack_mount(%s, %s, %s) executing...", "/system/bin/hijack", "/dev/block/mmcblk1p7", "/pds");
                result = hijack_mount("/system/bin/hijack", "/dev/block/mmcblk1p7", "/pds");
hijack_log("      returned: %d", result);

                // this will prevent hijack from being called again
hijack_log("    hijack_umount(%s, %s) executing...", "/system/bin/hijack", "/system");
                result = hijack_umount("/system/bin/hijack", "/system");
hijack_log("      returned: %d", result);

                char * updater_args[] = { UPDATE_BINARY, "2", "0", RECOVERY_UPDATE_ZIP, NULL };
hijack_log("    exec(\"%s %s %s %s\") executing...", UPDATE_BINARY, "2", "0", RECOVERY_UPDATE_ZIP);
                result = exec_and_wait(updater_args);
hijack_log("      returned: %d", result);

                return result;

            } 
    }

    char real_executable[PATH_MAX];
    sprintf(real_executable, "%s.bin", hijacked_executable);
    char ** argp = (char **)malloc(sizeof(char *) * (argc + 1));
    for (i = 0; i < argc; i++) {
        argp[i]=argv[i];
    }
    argp[argc] = NULL;

    argp[0] = real_executable;

hijack_log("  Executing real program: %s", real_executable);
    for (i = 0; i < argc; i++) {
hijack_log("    argp[%d] = \"%s\"", i, argp[i]);
    }

    // should clean up memory leaks, but it really doesn't matter since the process immediately exits.
    result = exec_and_wait(argp);
hijack_log("    returned: %d", result);

    return result;
}


