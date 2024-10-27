n: int = 5  # Number of processes
m: int = 3  # Number of resource types

# arrays set globally, values are set in reset_processes()
available: list[int] = []
maximum: list[list[int]] = []
allocation: list[list[int]] = []
need: list[list[int]] = []

# reset each processes resources for each test case
def reset_processes() -> None:
    global available 
    global maximum
    global allocation
    global need

    # Available Vector (initially total resources available)
    available = [3, 3, 2]

    # Maximum Matrix
    maximum = [
        [7, 5, 3],
        [3, 2, 2],
        [9, 0, 2],
        [2, 2, 2],
        [4, 3, 3]
    ]

    # Allocation Matrix
    allocation = [
        [0, 1, 0],
        [2, 0, 0],
        [3, 0, 2],
        [2, 1, 1],
        [0, 0, 2]
    ]

    # Need Matrix (Max - Allocation)
    need = [
        [7, 4, 3],
        [1, 2, 2],
        [6, 0, 0],
        [0, 1, 1],
        [4, 3, 1]
    ]

def is_safe() -> bool:
    # set work array to available
    work: list[int] = [x for x in available]
    # set finish array to [False] * number of processes
    finish: list[bool] = [False] * n
    # result array
    sequence: list[int] = []

    for _ in range(n):
        for i in range(n):
            # check to see finish[i] is false and each resource in need is <= the work
            if not finish[i] and all(need[i][k] <= work[k] for k in range(m)):
                finish[i] = True
                # calculate work: each work resource + each allocation resource at alloc.[i]
                work = [work[k] + allocation[i][k] for k in range(m)]
                # add to result array
                sequence.append(i)

    if (all(finish)):
        print("System is in a safe state.\n"
              f"Safe Sequence: {sequence}")
    else:
        print("Error: Not enough resources available.")

    return all(finish) 

def request_resources(process: int, request: list[int]) -> None:
    global available
    
    # check to see if the resources requested is less than or equal to the needed resources
    # of the process as well as less than or equal to the resources available for the process
    if all(request[i] <= need[process][i] for i in range(m)):
        if all(request[i] <= available[i] for i in range(m)):
            # set available resources for the process to current available - requested
            available = [available[i] - request[i] for i in range(m)]
            # set allocated resources for the process to currently allocated + requested
            allocation[process] = [allocation[process][i] + request[i] for i in range(m)]
            # set needed resources for the process to currently needed + requested
            need[process] = [need[process][i] - request[i] for i in range(m)]
        # if not enough resources available
        else:
            print(f"Error: process {process} must wait, resources are not available.") 
            return
    # if request goes above need claim
    else:
        print("Error: process has exceeded its maximum claim.")
        return

    # check to see if system is in safe state
    if is_safe():
        print(f"Resources allocated to process {process}.")
    else:
        reset_processes()

def main():
    while True:
        reset_processes()
        test_num = input("\n1. Test case 1: Run safe test\n"
                         "2. Test case 2: Process 1 requests resources [1, 0, 2]\n"
                         "3. Test case 3: Process 4 requests resources [3, 3, 1]\n"
                         "4. Quit\n") 
        try:
            test_num = int(test_num)
            if (test_num <= 0 or test_num >= 5):
                raise(ValueError)
        except ValueError:
            print("Input must be 1-4")
            continue

        # test cases
        match (test_num):
            # Run safe test
            case 1:
                is_safe()
            # Process 1 requests resources [1, 0, 2]
            case 2:
                request_resources(1, [1, 0, 2])
            # Process 4 reqiests resources [3, 3, 1]
            case 3:
                request_resources(4, [3, 3, 1])
            case 4:
                print("Exiting.")
                break

main()

