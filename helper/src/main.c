#define _POSIX_C_SOURCE 200809L

#include "../helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int loglevel = LOG_INFO;
int logfd = STDERR_FILENO;

#define DEFAULT_DAEMON_PATH "/tmp/brutalshell.sock"

#define UNIT_SEP "\x1f"

int main(int argc, char *argv[]) {
    struct helper_config cfg;
    struct daemon_response *resp = NULL;
    char helper_session[256];
    char *request_msg = NULL;
    size_t req_len = 0;
    int i, confirm = 0;
    int ret = EXIT_SUCCESS;

    memset(&cfg, 0, sizeof(struct helper_config));
    cfg.daemon_fd = -1;
    cfg.daemon_path = strdup(DEFAULT_DAEMON_PATH);
    cfg.argc = argc;
    cfg.argv = argv;

    if (!cfg.daemon_path) {
        print_error("malloc failed for daemon_path");
        return EXIT_FAILURE;
    }

    if (connect_daemon(&cfg) < 0) {
        print_error("failed to connect to daemon");
        ret = EXIT_FAILURE;
        goto cleanup;
    }

    char helper_session_buf[256];
    if (get_session(cfg.daemon_fd, helper_session_buf, sizeof(helper_session_buf)) < 0) {
        print_error("failed to get session ID from daemon");
        ret = EXIT_FAILURE;
        goto cleanup;
    }
    strncpy(cfg.helper_session, helper_session_buf, sizeof(cfg.helper_session) - 1);

    char *wrapper_session_env = getenv("BSH_TERM_SESSION");
    if (wrapper_session_env) {
        strncpy(cfg.wrapper_session, wrapper_session_env, sizeof(cfg.wrapper_session) - 1);
    } else {
        strcpy(cfg.wrapper_session, "NO_SESSION");
    }

    req_len =
        strlen(helper_session_buf) + 1 + strlen(wrapper_session_env) + 1;

    for (int i = 1; i < argc; i++) {
      req_len += strlen(argv[i]) + 1;
    }
    req_len += 1; // for null terminator

    request_msg = malloc(req_len);
    if (!request_msg) {
      print_error("malloc failed");
      ret = EXIT_FAILURE;
      goto cleanup;
    }

    strcpy(request_msg, helper_session_buf);
    strcat(request_msg, UNIT_SEP);

    strcat(request_msg, wrapper_session_env);
    strcat(request_msg, UNIT_SEP);
    for (int i = 1; i < argc; i++) {
      strcat(request_msg, argv[i]);
      if (i < argc - 1) {
        strcat(request_msg, " ");
      }
    }

    if (send_to_daemon(cfg.daemon_fd, request_msg, strlen(request_msg)) < 0) {
        print_error("failed to send request to daemon");
        ret = EXIT_FAILURE;
        goto cleanup;
    }

cleanup:
    if (request_msg) {
        free(request_msg);
    }
    cleanup_resources(&cfg, resp);
    return ret;
}
