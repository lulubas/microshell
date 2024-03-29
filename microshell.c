#include "microshell.h"
#include "stdio.h"

int print_err(char *error)
{
	while (*error)
	{
		write(STDERR_FILENO, error, 1);
		error++;
	}
	return (1);
}

int exec_cd(char **argv, int i)
{
	if (i != 2)
		return (print_err("error: cd: bad arguments\n"));
	printf("int i = %d argv[1]=%s\n", i, argv[1]);
	if (chdir(argv[1]) == -1)
	{
		print_err("error: cd: cannot change directory to ");
		print_err(argv[1]);
		print_err("\n");
		return (1);
	}
	return (0);
}

int	exec_cmd(char **argv, int i, char **envp)
{
	int pid;
	int pipe_fds[2];
	int is_piped = argv[i] && strcmp(argv[i], "|");

	if (is_piped && pipe(pipe_fds) == -1)
		return (print_err("error: fatal"));
	pid = fork();
	if (pid < 0)
		return (print_err("error: fatal"));
	else if (pid == 0)
	{
		if (is_piped && dup2(pipe_fds[1], STDOUT_FILENO) == -1 && close(pipe_fds[0]) == -1)
			return (print_err("error: fatal\n"));
		execve(argv[0], argv, envp);
		print_err("error: cannot execute ");
		print_err(argv[0]);
		print_err("\n");
		return (1);
	}
	else
	{
		waitpid(pid, NULL, 0);
		if (is_piped && dup2(pipe_fds[0], STDOUT_FILENO) == -1 && close(pipe_fds[0]) == -1 && close(pipe_fds[1]) == -1)
			return (print_err("error: fatal\n"));
		return(0);
	}
	return (1);
}

int main(int argc, char **argv,char **envp)
{
	int		i = 0;
	int		status = 0;

	if (argc < 2)
		return (0);
	while (argv[i] && argv[++i])
	{
		argv += i;
		i = 0;
		while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
			i++;
		if (!strcmp(*argv, "cd"))
			status = exec_cd(argv, i);
		else
			status = exec_cmd(argv, i, envp);
	}
	return (status);
}
