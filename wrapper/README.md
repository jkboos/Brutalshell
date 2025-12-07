# CLI Wrapper for Brutalshell

This wrapper is to transfer terminal context to the daemon for interacting
with LLMs.

## Features

- Transfer terminal context (cwd, history, env) to daemon using UNIX sockets
- Execute commands received from daemon
- Handle user input and output display
- Cross-platform support (Unix-like shells)
