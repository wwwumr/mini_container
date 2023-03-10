#include <iostream>
#include <sched.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mount.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#define CGROUP_FOLDER "/sys/fs/cgroup/pids/container/"
#define concat(a, b) (a"" b)

const int STACK_SIZE = 65536;

char *stack_mem(int stackSize) {
	auto *stack = new(std::nothrow) char[stackSize];

	if (!stack) {
		std::cout << "Can' t alloc stack for container" << std::endl;
		exit(EXIT_FAILURE);
	}

	return stack + stackSize;
}

int run(const char *name) {
	char *_args[] = {(char *)name, nullptr};
	execvp(name, _args);
}

void setup_root(const char *folder) {
	chroot(folder);
	chdir("/");	
}

void setup_variables() {
	clearenv();
	setenv("TERM", "xterm-256color", 0);
	setenv("PATH", "/bin/:/sbin/:/usr/bin/:/usr/sbin/", 0);	
}

int runThis(void *args) {
	run("/bin/sh");	
}

void write_rule(const char *path, const char *value) {
	int fp = open(path, O_WRONLY, O_APPEND);
	write(fp, value, strlen(value));
	close(fp);	
}

void limitProcess() {
	mkdir(CGROUP_FOLDER, S_IRUSR | S_IWUSR);

	const char *pid = std::to_string(getpid()).c_str();

	write_rule(concat(CGROUP_FOLDER, "cgroup.procs"), pid);
	write_rule(concat(CGROUP_FOLDER, "pids.max"), "5");
	write_rule(concat(CGROUP_FOLDER, "notify_on_release"), "1");
}

int jail(void *args) {
	limitProcess();
	setup_variables();
	setup_root("./rootfs");
	std::cout << "Hello from Child" << getpid() << std::endl;

	mount("proc", "/proc", "proc", 0, 0);

	auto pid = clone(runThis, stack_mem(STACK_SIZE), SIGCHLD, 0);


	wait(nullptr);
	umount("/proc");

	return EXIT_FAILURE;
}

int main() {
	std::cout << "Hello from Parent: " << getpid() << std::endl;
	clone(jail, stack_mem(STACK_SIZE), CLONE_NEWUTS | CLONE_NEWPID | SIGCHLD, 0);
	
	wait(nullptr);
	return EXIT_SUCCESS;
}


