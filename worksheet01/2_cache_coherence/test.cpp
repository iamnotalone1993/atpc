#include <cstdio>	// printf...
#include <mpi.h>	// MPI_...

template<typename T>
class DistObj
{
private:
	T		_obj;
	MPI_Comm	comm;

	int 		num_procs;
	int		my_pid;
	int		token_count;

public:
	// Collective constructor: all members of comm have to call
	// the constructor and all have to pass the same value for val.
	// I.e., val is the coherent initial value for all instances of the object
	DistObj(const T& val, const MPI_Comm& comm);
	~DistObj();

	T read();			// read the current value

	void write(const T& val);	// update the current value
};

template<typename T>
DistObj<T>::DistObj(const T& val, const MPI_Comm& comm)
	: _obj(val), comm(comm), token_count{1}
{
	int provided;
	MPI_Init_thread(NULL, NULL, MPI_THREAD_MULTIPLE, &provided);
	MPI_Comm_size(comm, &num_procs);
	MPI_Comm_rank(comm, &my_pid);
	MPI_Barrier(comm);
}

template<typename T>
DistObj<T>::~DistObj()
{
	MPI_Finalize();
}

template<typename T>
T DistObj<T>::read()
{
	MPI_Status	status;
	MPI_Request	request;
	int		buf,
			flag;

	// check if some other process is requesting my token
	MPI_Irecv(&buf, 1, MPI_INT,
			(my_pid + num_procs - 1) % num_procs, 0, comm, &request);
	MPI_Test(&request, &flag, &status);

	// if yes, then...
	if (flag)
	{
		// give my token to some other process
		MPI_Send(&buf, 1, MPI_INT,
				(my_pid + num_procs + 1) % num_procs, 0, comm);

		// synchronize the distributed object
		MPI_Recv(&_obj, sizeof(int), MPI_BYTE,
				(my_pid + num_procs - 1) % num_procs, 1, comm, &status);
		MPI_Send(&_obj, sizeof(int), MPI_BYTE,
				(my_pid + num_procs + 1) % num_procs, 1, comm);
	}
	else // otherwise, then...
	{
		// cancel the recv message posted
		MPI_Cancel(&request);
		MPI_Wait(&request, &status);
	}

	// return the current value of the distributed object
	return _obj;
}

template<typename T>
void DistObj<T>::write(const T& val)
{
	MPI_Status	status;
	int 		buf = my_pid;
	
	// get all tokens
	MPI_Send(&buf, 1, MPI_INT,
			(my_pid + num_procs + 1) % num_procs, 0, comm);
	MPI_Recv(&buf, 1, MPI_INT,
			(my_pid + num_procs - 1) % num_procs, 0, comm, &status);

	// distribute the value of the object
	_obj = val;
	MPI_Send(&_obj, sizeof(T), MPI_BYTE,
			(my_pid + num_procs + 1) % num_procs, 1, comm);
	MPI_Recv(&_obj, sizeof(T), MPI_BYTE,
			(my_pid + num_procs - 1) % num_procs, 1, comm, &status);
}

int main()
{
	MPI_Comm comm = MPI_COMM_WORLD;
	DistObj<int> dist_obj(1, comm);

	int my_pid;
	MPI_Comm_rank(comm, &my_pid);

	int	output,
	    	counter = 3;
	if (my_pid == 0)
	{
		output = dist_obj.read();
		printf("[%d]output = %d\n", my_pid, output);

		dist_obj.write(10);

		output = dist_obj.read();
		while (counter > 0)
		{
			printf("[%d]output = %d\n", my_pid, output);
			--counter;
		}
	}
	else // the others
	{
		while (true)
		{
			output = dist_obj.read();
			if (output == 1)
				printf("[%d]output = %d\n", my_pid, output);
			if (output != 1 && counter > 0)
			{
				printf("[%d]output = %d\n", my_pid, output);
				--counter;
			}
		}
	}

	return 0;
}

