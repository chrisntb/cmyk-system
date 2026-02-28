# Nodes

Make sure you are using nodes that have large enough disks.
After installing K8s the node disk usage `sudo du -h -d 1 /` was as follows:

- Private Bare Metal, Ubuntu 24.04 LTS
  - Controller `15G`
  - Worker - Compute `20G`
  - Worker - GPU `36G`
- Multipass Virtual Machines, Ubuntu 24.04 LTS
  - Controller `5.7G`
  - Worker - Compute `5G`

Make sure you are using nodes that have enough cores and memory.
We received these errors when initially testing with very small virtual machines:

```shell
[ERROR NumCPU]: the number of available CPUs 1 is less than the required 2
[ERROR Mem]: the system RAM (955 MB) is less than the minimum 1700 MB
```

## Private Bare Metal Nodes

In order to run the playbooks you will need:

- Your public key added to the proxy you intend to use
- Aliases in your `${HOME}/.ssh/config`

```text
Include ~/.ssh/ssh_config_proxy.inc
```

For example, see [../etc/ssh_config_proxy.inc](../etc/ssh_config_proxy.inc).

## Multipass Virtual Machines Nodes

Using Canonical's Multipass, create these nodes; this is sufficient for initial development:

```shell
multipass launch -n ctl       -c 4 -d 10G -m 4G 24.04
multipass launch -n wrk-hpc-1 -c 4 -d 15G -m 4G 24.04
multipass launch -n wrk-hpc-2 -c 4 -d 15G -m 4G 24.04

# At first support a client interface to the cluster - no mgt services required
#multipass launch -n wrk-mgt-1 -c 4 -d 10G -m 4G 24.04

multipass list
# Name                    State             IPv4             Image
# ctl                     Running           10.38.15.5       Ubuntu 24.04 LTS
# wrk-hpc-1               Running           10.38.15.11      Ubuntu 24.04 LTS
# wrk-hpc-2               Running           10.38.15.41      Ubuntu 24.04 LTS
```

Then add your public key to the nodes which will enable you to use Ansible to provision a K8s cluster:

```shell
YOUR_PUBLIC_KEY=$(cat ${HOME}/.ssh/id_ed25519.pub)

multipass exec ctl       -- bash -c "echo ${YOUR_PUBLIC_KEY} >> .ssh/authorized_keys"
multipass exec wrk-hpc-1 -- bash -c "echo ${YOUR_PUBLIC_KEY} >> .ssh/authorized_keys"
multipass exec wrk-hpc-2 -- bash -c "echo ${YOUR_PUBLIC_KEY} >> .ssh/authorized_keys"

# At first support a client interface to the cluster - no mgt services required
#multipass exec wrk-mgt-1 -- bash -c "echo ${YOUR_PUBLIC_KEY} >> .ssh/authorized_keys"
```

Try some ad hoc commands:

```shell
ansible -i inventory/dev/hosts.yaml cluster -m command -a 'cat /etc/hosts'
ansible -i inventory/dev/hosts.yaml cluster -m command -a 'ip -br a'
```

Note when Multipass creates a node, it adds a public SSH key to the node's `.ssh/authorized_keys` file:

```shell
sudo ssh-keygen -y -f /var/snap/multipass/common/data/multipassd/ssh-keys/id_rsa
```

In order to run the playbooks you will need:

- Your public key added to all hosts in the `inventory`
  - See instructions above
- Aliases in your `${HOME}/.ssh/config`

```text
Include ~/.ssh/ssh_config.inc
```

For example, see [../etc/ssh_config.inc](../etc/ssh_config.inc).
