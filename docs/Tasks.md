# Tasks

## Prerequisites

- Install the required tools
  - [Tools.md](Tools.md)
- Setup your environment
  - [Setup.md](Setup.md)
- Create nodes
  - [Nodes.md](Nodes.md)

Initialize the virtual environment:

```shell
# Note 'uv' automatically infers the virtual environment
uv sync

# Activate the virtual environment
# -> For this shell alias see the 'Prerequisites' instructions
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

Check tools:

```shell
ansible --version
ansible-playbook --version
```

Try some ad hoc commands:

```shell
ansible -i inventory/dev/hosts.yaml cluster -m command -a 'cat /etc/hosts'
ansible -i inventory/dev/hosts.yaml cluster -m command -a 'ip -br a'
```

Run the node health checks:

```shell
ansible-playbook -i inventory/dev/hosts.yaml playbooks/checks/01_ping-hosts_plays.yaml
```

## Create K8s Cluster

See [K8s Cluster](k8s/README.md).

## Add Management Services

See [Management Services](mgt/README.md).
