//add -ldl when compiling
//put argument in parentheses like this: 'atan(x)' -5.0 5.0 10000
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dlfcn.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

const int WRONG_ARGUMENTS = 1;
const int PIPE_ERROR = 2;
const int DUP2_ERROR = 3;
const int GCC_ERROR = 4;
const int COMPILATION_ERROR = 5;
const int DLOPEN_ERROR = 6;
const int FINDING_ADDRESS_ERROR = 7;
const int GNU_PLOT_ERROR = 8;

int main (int argc, char *argv [])
{
	if (argc != 5)
	{
		fprintf (stderr, "Usage: %s <function(x)> <x_min> <x_max> <quantity>\n", argv [0]);
		return WRONG_ARGUMENTS;
	} 
	double x_min = strtod (argv [2], NULL);
	double x_max = strtod (argv [3], NULL);
	int quantity = atoi (argv [4]);


	int gcc_pipe [2];
	if (pipe (gcc_pipe) < 0) //create pipe that will be used to send source code to gcc
	{
		perror ("pipe");
		return PIPE_ERROR;
	}

	pid_t gcc_pid;
	if ((gcc_pid = fork ()) == 0) //check if child process is executing
	{
		close (gcc_pipe [1]); //we don't need pipe input in child process
		if (dup2 (gcc_pipe [0], fileno (stdin)) < 0) //connect pipe output to stdin
		{
			perror ("dup2");
			close (gcc_pipe [0]);
			return DUP2_ERROR;
		}
		close (gcc_pipe [1]); //close old FD

		//start GCC
		//execlp usage: int execlp(const char *file, const char *arg, ... /* (char  *) NULL */)
		execlp ( 
			//executable
			"gcc",
			//argv [0], argv [1], ...
			"gcc", "-Wall", "-Wextra",
			//produce position-independent dynamic library (shared object)
			"-fPIC", "-fPIE", "-shared",
			//enable optimization
			"-O2", "-fomit-frame-pointer", "-march=native", "-mtune=native",
			//read input from stdin, treat it as a C source code
			"-xc", "-",
			//link with libm
			"-lm",
			//save output to tmp.so
			"-o", "tmp.so",
			//terminating NULL
			NULL
		);
		perror ("EXEC GCC");
		return GCC_ERROR;
	}

	//code executed by parent process
	close (gcc_pipe [0]); //we don't need pipe output in parent process
	//FILE* source_fp = fdopen (gcc_pipe [1], "w");
	dprintf (gcc_pipe [1], 
		"#include <math.h>\n"
		"double f (double x)\n"
		"{ return %s; }\n",
		argv [1]);
	close (gcc_pipe [1]);
	
	//wait for compilation and check its status
	int gcc_status;
	waitpid (gcc_pid, &gcc_status, 0);
	if ((!WIFEXITED (gcc_status)) || (WEXITSTATUS (gcc_status) != 0))
	{
		printf("Compilation error, gcc exit code: %d\n", WEXITSTATUS (gcc_status));
		return COMPILATION_ERROR;
	}

	//load compiled library
	void *tmplib = dlopen ("./tmp.so", RTLD_LAZY); //get handle for the loaded object
	char *dlerrstr;
	if (!tmplib) //check if pointer is NULL
	{
		perror (dlerror ());
		return DLOPEN_ERROR;
	}

	//find address of loaded function
	double (*tmp_function) (double);
	tmp_function = dlsym (tmplib, "f");
	if ((dlerrstr = dlerror ()) != NULL)
	{
		puts (dlerrstr);
		dlclose (tmplib);
		return FINDING_ADDRESS_ERROR;
	}

	const double delta = (x_max - x_min) / quantity;

	//---------------------graph------------------------------------------
	//open pipe to gnu plot using popen
	FILE *gnuplot_pipe = popen ("gnuplot -persist", "w"); //"w" means that pipe was opened only for writing
														  //"-persist" will keep the gnuplot window opened until manually closed
	if (!gnuplot_pipe) 
	{
		perror ("Failed to start gnuplot");
		return GNU_PLOT_ERROR;
	}

	//graph options
	//fprintf (gnuplot_pipe, "set output 'graphs/%s.png'\n", argv [1]);
    fputs ("set xlabel 'X'\n", gnuplot_pipe);
    fputs ("set ylabel 'Y'\n", gnuplot_pipe);
    fputs ("set grid\n", gnuplot_pipe);
    //fputs ("set xrange [-4*pi:4*pi]\n", gnuplot_pipe);
    //fputs ("set yrange [-1.5:1.5]\n", gnuplot_pipe);
    fprintf (gnuplot_pipe, "set title 'f(x)=%s'\n", argv [1]);
    fputs ("plot '-' w line\n", gnuplot_pipe);

	for (int i = 0; i < quantity; i++)
	{
		double x = x_min + i * delta;
		fprintf (gnuplot_pipe, "%.10lf %.10lf\n", x, (*tmp_function) (x)); //write graph points to pipe
	}
	pclose (gnuplot_pipe); //free resources
	//---------------------graph------------------------------------------

	//integration
	//rect
	double sum = 0;
	for (int i = 0; i < quantity - 1; i++)
	{
		double x = x_min + (i + 0.5) * delta; //between x(i) and x(i+1)
		sum += ((*tmp_function) (x)) * delta; //write graph points to pipe
	}
	printf ("%.10lf\n", sum);

	dlclose (tmplib); //free resources
	return 0;
}
