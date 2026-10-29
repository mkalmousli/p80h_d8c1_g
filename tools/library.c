/* SPDX-License-Identifier: MIT */
#define _POSIX_C_SOURCE 200809L
#include "web.h"
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* Only tracked, policy-checked text is published. local/ and out/ are never read. */
static int document(const char *path, FILE *index)
{
    char output[1024];
    if (strspn(path, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_/.") != strlen(path) ||
        path[0] == '/' || strstr(path, "..") || !strncmp(path, "local/", 6)) return 1;
    int n = snprintf(output, sizeof(output), "out/site/files/%s.html", path);
    if (n < 0 || (size_t)n >= sizeof(output) || make_parents(output)) return 1;
    FILE *in = fopen(path, "r"), *out = fopen(output, "w");
    if (!in || !out) { if (in) fclose(in); if (out) fclose(out); return 1; }
    page_head(out, path);
    const char *extension = strrchr(path, '.');
    int is_markdown = extension && !strcmp(extension, ".md");
    fputs("<p class=eyebrow>REPOSITORY LIBRARY</p><p class=filepath>", out); html(out, path);
    fputs("</p><article>", out);
    int failed = 0;
    if (is_markdown) failed = markdown(in, out, path);
    else {
        fputs("<pre class=source><code>", out);
        int c;
        while ((c = fgetc(in)) != EOF) { char one[] = {(char)c, 0}; html(out, one); }
        fputs("</code></pre>", out);
    }
    fputs("</article>", out); page_foot(out);
    failed |= ferror(in) || ferror(out);
    fclose(in); failed |= fclose(out) != 0;
    fputs("<li><a href=\"files/", index); html(index, path); fputs(".html\">", index);
    html(index, path); fputs("</a></li>\n", index);
    return failed;
}

int library_pages(void)
{
    int pipes[2], status, failed = 0;
    if (pipe(pipes)) return 1;
    pid_t pid = fork();
    if (pid < 0) { close(pipes[0]); close(pipes[1]); return 1; }
    if (!pid) {
        close(pipes[0]);
        if (dup2(pipes[1], STDOUT_FILENO) < 0) _exit(1);
        close(pipes[1]); execlp("git", "git", "ls-files", (char *)NULL); _exit(127);
    }
    close(pipes[1]);
    FILE *paths = fdopen(pipes[0], "r"), *index = fopen("out/site/library.html", "w");
    if (!paths || !index) { if (paths) fclose(paths); else close(pipes[0]); return 1; }
    page_head(index, "Repository library");
    fputs("<p class=eyebrow>DOCS / DATA / CODE</p><h1>The project, readable.</h1>"
          "<p>Generated from every tracked text file. Documentation is rendered; "
          "source and data remain inspectable. Private files are never included.</p>"
          "<p><a href=files/README.md.html>Start here</a> · "
          "<a href=files/docs/status.md.html>Bring-up journal</a></p><ul class=filelist>", index);
    char path[1024];
    while (fgets(path, sizeof(path), paths)) {
        if (!strchr(path, '\n')) { failed = 1; continue; }
        path[strcspn(path, "\n")] = 0;
        const char *ext = strrchr(path, '.');
        if (ext && !strcmp(ext, ".png")) continue;
        failed |= document(path, index);
    }
    failed |= ferror(paths); fclose(paths);
    if (waitpid(pid, &status, 0) != pid || !WIFEXITED(status) || WEXITSTATUS(status)) failed = 1;
    fputs("</ul>", index); page_foot(index);
    failed |= ferror(index); failed |= fclose(index) != 0;
    return failed;
}
