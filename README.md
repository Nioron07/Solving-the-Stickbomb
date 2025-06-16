# Stickbomb Matrix Builder

This project is an interactive matrix-based tool that helps construct valid stick-based structures. It simulates the logical assembly process of a stickbomb by building and updating a connection matrix based on user inputs.

## What It Does

- Prompts the user to enter a number of sticks (minimum of 4).
- Constructs a square matrix that represents the connectivity between each node of every stick.
- Accepts coordinate-based connections from the user.
- Updates the matrix by recording the connection and automatically disabling all newly invalid connections.
- Repeats the process until all possible connections have been made.

## How It Works

Each stick is internally represented as three nodes (two ends and one middle). The matrix is initialized with default values:
- `"0"`: No connection yet
- `"1"` / `"-1"`: Represents layered connection (Weak Bond)
- `"2"`: Represents physical connection (Strong Bond)
- `"x"`: Connection disallowed due to internal logic or invalid combinations

As the user builds connections, the matrix:
- Records the new connection with a `"1"` and its opposite as `"-1"`
- Eliminates all other conflicting or now-impossible connections by setting them to `"x"`

## How to Use

1. **Compile and Run**

   Make sure you have a C++ compiler. Example with `g++`:

   ```bash
   g++ -std=c++17 -Iinclude src/main.cc src/matrix.cc -o bin/output
   ./bin/output.exe
   ```

2. **Follow the Prompts**

   - Enter the number of sticks you'd like to simulate (must be 4 or more).
   - You will then be asked to enter two node numbers (from 1 to *n*, where *n = 3 × number of sticks*).
   - Input the operation direction:
     - `+` to indicate the first node is above the second
     - `-` to indicate the first node is under the second

3. **See the Matrix**

   After each valid connection, the matrix is automatically updated and printed. Each cell tells you:
   - `"0"`: Still available
   - `"1"` / `"-1"`: Connected (directional)
   - `"x"`: Blocked connection
   - `"-"` / `"+"`: Sign bounds on that specific connection. Any newly proposed connection must be the same sign.

4. **Repeat Until Complete**

   Keep adding valid connections until all connectable cells are filled.

## Matrix Output

The matrix is printed after each connection, showing which nodes are connected, unconnected, or blocked.

```
     1   2   3   4   5   6 ...
 1 | x   2   x   0   0   0 ...
 2 | 2   x   2   0   0   0 ...
 3 | x   2   x   0   0   0 ...
...
```

## Example Session
the following sequence of inputs creates a 4 stick matrix (12 x 12) and creates a positive End to Middle connection between node 1 and node 5

```
Enter number of sticks (>=4): 4

Initial matrix:
     |   1   2   3   4   5   6   7   8   9  10  11  12
-----+------------------------------------------------
   1 |   x   2   x   0   0   0   0   0   0   0   0   0
   2 |   2   x   2   0   0   0   0   0   0   0   0   0
   3 |   x   2   x   0   0   0   0   0   0   0   0   0
   4 |   0   0   0   x   2   x   0   0   0   0   0   0
   5 |   0   0   0   2   x   2   0   0   0   0   0   0
   6 |   0   0   0   x   2   x   0   0   0   0   0   0
   7 |   0   0   0   0   0   0   x   2   x   0   0   0
   8 |   0   0   0   0   0   0   2   x   2   0   0   0
   9 |   0   0   0   0   0   0   x   2   x   0   0   0
  10 |   0   0   0   0   0   0   0   0   0   x   2   x
  11 |   0   0   0   0   0   0   0   0   0   2   x   2
  12 |   0   0   0   0   0   0   0   0   0   x   2   x

Add a connection
Enter the first number (1-12): 1
Enter the second number (1-12): 5
Enter operation ('+' or '-'): +

Current matrix:
     |   1   2   3   4   5   6   7   8   9  10  11  12
-----+------------------------------------------------
   1 |   x   2   x   x   1   x   0   0   0   0   0   0
   2 |   2   x   2   +   x   +   0   0   0   0   0   0
   3 |   x   2   x   x   x   x   0   0   0   0   0   0
   4 |   x   -   x   x   2   x   0   0   0   0   0   0
   5 |  -1   x   x   2   x   2   0   0   0   0   0   0
   6 |   x   -   x   x   2   x   0   0   0   0   0   0
   7 |   0   0   0   0   0   0   x   2   x   0   0   0
   8 |   0   0   0   0   0   0   2   x   2   0   0   0
   9 |   0   0   0   0   0   0   x   2   x   0   0   0
  10 |   0   0   0   0   0   0   0   0   0   x   2   x
  11 |   0   0   0   0   0   0   0   0   0   2   x   2
  12 |   0   0   0   0   0   0   0   0   0   x   2   x
```

## Input Validation

- The program ensures that numbers are within range.
- Prevents duplicate or invalid connections.
- Rejects malformed inputs and prompts for retry.

## File Structure

- `Matrix.hpp` / `Matrix.cc`: Core logic for matrix handling, user input, and updating
- `main.cc`: Entry point and loop driver
- `/bin`: Folder for output.exe 
