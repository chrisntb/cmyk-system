# K8s - Architecture

K8s is a system for automating the deployment, scaling, and management of containerized applications.
Described here are the building blocks that make K8s work and how to use them to create an ad hoc cluster.
Testing the cluster is also covered.

## Container Runtimes

A container runtime e.g. `CRI-O` or `containerd` needs to be available on each node in order for pods to run.
`CRI-O` and `Containerd` handle high-level tasks such as image pulling, storage management, and `pod` lifecycle coordination before delegating to low-level runtimes like `Runc` or `gVisor`.
The OCI reference low-level runtimes focus solely on executing individual containers via Linux primitives like `namespaces` and `cgroups`.

The following container runtimes have been tested:

- `CRI-O`
  - `https://cri-o.io/`
  - Minimum feature set to support K8s
- `containerd`
  - `https://containerd.io/`
  - Supports orchestrators in addition to K8s e.g. Docker

## Namespaces & Control Groups

On Linux, `namespaces` are used to isolate a process and `cgroups` are used to constrain resources that are allocated to processes e.g. memory, cpu cycles etc.

There are two drivers for interfaces with control groups: `cgroupgfs` and `systemd`. `cgroupgfs` used to be the default driver used by `kubelet` but now, with `cgroup v2`, `systemd` is.

When `systemd` is used for the `init` system by the Linux distribution, it will be the `cgroup` manager and therefore should (must) be used by `kubelet`.
The `init` system will create a `cgroup` per `systemd` unit and if `kubelet` does not use this `cgroup`, and instead allocates an additional `cgroup`, there can be instability.

`kubelet` instructs the container runtime to set the control group limits and the container runtime sets the configuration which is then enforced by the kernel.

Both the container runtime and `kubelet` need to use the same control group driver because although it is the container runtime's job so set the control group configuration, `kubelet` interfaces with the control groups for monitoring.

## Container Network Interface

Container Network Interface (CNI) specifies network interface in a cluster and is loaded by the container runtime.

The following CNI implementations have been tested:

- Flannel
  - `https://github.com/flannel-io/flannel`
  - Single binary agent called flanneld on each host, responsible for allocating a subnet lease to each host out of a larger, preconfigured address space

## K8s Services and Tools

### crictl

CLI tool for working with the container runtime.

### kubelet

kubelet is the agent that runs on each node and ensures containers are running in a pod.
It works with the k8s control plane to transalate PodSpecs (desired state) into CRI calls (gRPC) to the container runtime which then start/stops the container. kubelet also manages container lifecycle i.e. crashes etc.

### kubeadm

CLI tool for bootstrapping K8s clusters, handling initialization of control plane nodes (kubeadm init) and joining worker nodes (kubeadm join).

### kubectl

Kubectl provides a command-line interface for interacting with the Kubernetes API, enabling users to create, inspect, update, or delete resources like pods, deployments, and services across the cluster. It is only initialized on the controller node.

## Nvidia GPU Operator

The Nvidia GPU Operator is used to integrate Nvidia GPUs with K8s, including GPU sharing `https://www.nvidia.com/en-us/technologies/multi-instance-gpu/` and monitoring `https://developer.nvidia.com/dcgm`.

## Scheduling & Queuing

Use the native scheduler enhanced with Kueue for job admission and placement, or replace the native K8s scheduler for certain workloads with the KAI Scheduler.

### Kueue

The Kueue enhances the native K8s scheduler with job admission and placement logic. A good overview is available here: `https://www.coreweave.com/blog/kueue-a-kubernetes-native-system-for-ai-training-workloads`.

Kueue manages quotas and how jobs consume them.
Kueue decides when a job should wait, when a job should be admitted to start (as in pods can be created) and when a job should be preempted (as in active pods should be deleted).
It supports placement based on resource flavours that can be a combination of CPU, Memory, GPU. Network Fabric and other resources, see `https://kueue.sigs.k8s.io/docs/overview/`.
Like the KAI Scheduler, it supports gang scheduling AKA ‘All-or-nothing’ and various fair sharing mechanisms.

### KAI Scheduler

The KAI scheduler replaces the native K8s scheduler for certain workloads.
It is often discussed in terms of its usefulness for scheduling fractional work onto a GPU i.e. two jobs running on one GPU.
This feature works by scheduling a 'reservation pod' onto a GPU which there-after is responsible for scheduling real work.
The real work has only soft isolation, and so for non-inference workloads this feature is only useful in trusted environments.
Note that the Nvidia GPU Operator also allows work to be scheduled on GPUs, just not fractional work.
However the Nvidia GPUs we are using all support [Multi-Instance GPU (MIG)](https://www.nvidia.com/en-us/technologies/multi-instance-gpu/), which exposes fractions of a GPU as if they were independent GPUs, and these can be scheduled independently by the Nvidia GPU Operator.
For us, the KAI scheduler's fractional GPU support is not that interesting.
What is interesting is the advanced scheduling and management features it makes available to a K8s cluster:

- Gang scheduling
- Bin-Packing
  - Packs pods tightly onto fewer nodes to consolidate resources and reduce fragmentation, ideal for maximizing GPU utilization in dense clusters
- Spreading
  - Distributes pods across more nodes for fault tolerance, load balancing, or topology awareness
- Elastic scaling
  - Pods in a job or gang specify minPods and maxPods in their PodGroup spec; the KAI Scheduler schedules the minimum first, then adds more up to the max if cluster resources (like GPUs) permit, enabling bursty AI jobs to grow without overprovisioning.
  This integrates with autoscalers like Karpenter or Luna, which provision nodes only for eligible pods within these limits, optimizing costs by avoiding scaling for quota-blocked jobs.
- Hierarchical queues
  - Quota (guaranteed resources), limit (hard cap), over-quota weight (for surplus sharing), and priority
- Dominant Resource Fairness
  - Equitable distribution by identifying each queue's dominant resource (e.g. GPUs over CPU/memory) and balancing shares accordingly, triggering actions like reclamation (evicting over-users) or preemption to realign allocations.
  This prevents any queue from monopolizing GPUs in contended clusters, promoting fairness across tenants without manual intervention
