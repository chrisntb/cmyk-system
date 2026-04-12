# Setup

After cloning this repository, ensure you have the development tools available, see [Tools.md](Tools.md).

## Python Virtual Environment

```shell
# Add a convenient alias in your shell profile
# -> Note using it assumes you are in the relevent directoty that contains the venv
echo 'alias venv=". .venv/bin/activate"' >> ~/.bashrc
exec bash -l
# OR
echo 'alias venv=". .venv/bin/activate"' >> ~/.zshrc
exec zsh

# Create a virtual environment
uv venv

# Activate the virtual environment
# -> For this shell alias see above
venv

# Deactivate the virtual environment
deactivate
```

Check for outdated dependencies:

```shell
# Note 'uv' automatically infers the virtual environment
# -> You don't need to have the virtual environment active for the following cmds to work correctly

uv tree --outdated --depth=1

# If you updated dependencies versions in pyproject.toml
# -> Sync again to pin the new dependencies in file uv.lock
uv sync
```

## Notes

### Original Project Initialization

For reference ONLY.
This component was originally created using the instructions documented here.

Install tools, see [Tools.md](Tools.md).

Create project:

```shell
uv init cmyk-system --python 3.14
```

Customize `pyproject.toml`:

```toml
[project]
name = "cmyk-system"
version = "0.1.0"
description = "Infrastructure configuration management for CMYK"
authors = [
  {name = "chrisntb", email = "chrisntb@example.org"}
]
license = {text = "UNLICENSED"}
readme = "README.md"
requires-python = ">=3.14"
dependencies = [
  "ansible (==13.3.0)",
  "ansible-dev-tools (==26.1.0)"
]
```
