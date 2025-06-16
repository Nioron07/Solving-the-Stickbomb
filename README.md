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
- `"1"` / `"-1"`: User-defined directed connection
- `"2"`: Internal structure marker
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
     - `+` to indicate the first node connects into the second
     - `-` to indicate the first node connects from the second

3. **See the Matrix**

   After each valid connection, the matrix is automatically updated and printed. Each cell tells you:
   - `"0"`: Still available
   - `"1"` / `"-1"`: Connected (directional)
   - `"x"`: Blocked connection

4. **Repeat Until Complete**

   Keep adding valid connections until all connectable cells are filled.

## Example Session

```
Enter number of sticks (>=4): 4
Enter the first number (1 to 12): 2
Enter the second number (1 to 12): 8
Enter operation ('+' or '-'): +
```

This creates a connection from node 2 to node 8. The program then updates the matrix, marking the connection and disabling others as needed.

## Matrix Output

The matrix is printed after each connection, showing which nodes are connected, unconnected, or blocked.

```
     1   2   3   4   5   6 ...
 1 | x   2   x   0   0   0 ...
 2 | 2   x   2   0   0   0 ...
 3 | x   2   x   0   0   0 ...
...
```

## Input Validation

- The program ensures that numbers are within range.
- Prevents duplicate or invalid connections.
- Rejects malformed inputs and prompts for retry.

## File Structure

- `Matrix.hpp` / `Matrix.cpp`: Core logic for matrix handling, user input, and updating
- `main.cpp`: Entry point and loop driver
