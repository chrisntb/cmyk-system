# K8s Cluster Example - Tests

After you have setup the cluster, see [Setup](02_Setup.md), here are some basic tests that make sure the cluster can schedule work.
These tests use existing containers and override their entry point with injected logic.

> To run tests that schedule GPU work, you must have setup the Nvidia GPU Operator, see [Setup Nvidia GPU Operator](05_Setup_Nvidia-GPU-Operator.md)

> To run tests that demonstrate advanced queuing, you must setup the KAI scheduler and Kueue, see [Setup Kueue](03_Setup_Kueue.md) and [Setup KAI Scheduler](04_Setup_KAI-Scheduler.md)

All these tests use an Nvidia CUDA container even though they are not always scheduling work onto a GPU node.
We do this because we want to limit the number of distinct containers we pull down from DockerHub in order to avoid being rate limited.

> Run these tests from the Controller node

## Prerequisites

Set up common environment variables for the tests. Note this assumes proxy environment variables are available, see `/etc/environment`.

Create and apply ConfigMap:

```shell
# Note these environment variables are populated with their values on the Controller (EOF not 'EOF') but they are valid for all nodes
cat <<EOF | tee test-environment.yaml
apiVersion: v1
kind: ConfigMap
metadata:
  name: test-environment
data:
  http_proxy: "${http_proxy}"
  https_proxy: "${https_proxy}"
  no_proxy: "${no_proxy}"
EOF

kubectl apply -f test-environment.yaml
```

## Simple Shell Job - Display Environment

Create specification and schedule job:

```shell
cat <<'EOF' | kubectl apply -f -
apiVersion: v1
kind: Pod
metadata:
  name: test-container-environment
spec:
  restartPolicy: Never
  containers:
  - name: nvidia-cuda
    image: nvidia/cuda:13.0.2-base-ubuntu22.04
    envFrom:
      - configMapRef:
          name: test-environment
    command: ["/bin/sh", "-c"]
    args:
    - |
      set -e

      echo
      echo "-------- Printing environment --------"
      env
      echo "-"
      echo
    resources:
      limits:
        cpu: 1
EOF
```

Inspect the job:

```shell
kubectl get pods --all-namespaces -o wide
kubectl describe pod test-container-environment
kubectl logs test-container-environment
# kubectl delete pod test-container-environment
```

Expected logs:

```text
-------- Printing environment --------
NVIDIA_VISIBLE_DEVICES=all
KUBERNETES_SERVICE_PORT_HTTPS=443
KUBERNETES_SERVICE_PORT=443
no_proxy=localhost,127.0.0.1,10.245.0.0/16,11.11.1.1,11.11.1.2,11.11.1.3,.svc,.cluster.local
HOSTNAME=test-container-environment
NVIDIA_REQUIRE_CUDA=cuda>=13.0 brand=unknown,driver>=535,driver<536 brand=grid,driver>=535,driver<536 brand=tesla,driver>=535,driver<536 brand=nvidia,driver>=535,driver<536 brand=quadro,driver>=535,driver<536 brand=quadrortx,driver>=535,driver<536 brand=nvidiartx,driver>=535,driver<536 brand=vapps,driver>=535,driver<536 brand=vpc,driver>=535,driver<536 brand=vcs,driver>=535,driver<536 brand=vws,driver>=535,driver<536 brand=cloudgaming,driver>=535,driver<536 brand=unknown,driver>=550,driver<551 brand=grid,driver>=550,driver<551 brand=tesla,driver>=550,driver<551 brand=nvidia,driver>=550,driver<551 brand=quadro,driver>=550,driver<551 brand=quadrortx,driver>=550,driver<551 brand=nvidiartx,driver>=550,driver<551 brand=vapps,driver>=550,driver<551 brand=vpc,driver>=550,driver<551 brand=vcs,driver>=550,driver<551 brand=vws,driver>=550,driver<551 brand=cloudgaming,driver>=550,driver<551 brand=unknown,driver>=565,driver<566 brand=grid,driver>=565,driver<566 brand=tesla,driver>=565,driver<566 brand=nvidia,driver>=565,driver<566 brand=quadro,driver>=565,driver<566 brand=quadrortx,driver>=565,driver<566 brand=nvidiartx,driver>=565,driver<566 brand=vapps,driver>=565,driver<566 brand=vpc,driver>=565,driver<566 brand=vcs,driver>=565,driver<566 brand=vws,driver>=565,driver<566 brand=cloudgaming,driver>=565,driver<566 brand=unknown,driver>=570,driver<571 brand=grid,driver>=570,driver<571 brand=tesla,driver>=570,driver<571 brand=nvidia,driver>=570,driver<571 brand=quadro,driver>=570,driver<571 brand=quadrortx,driver>=570,driver<571 brand=nvidiartx,driver>=570,driver<571 brand=vapps,driver>=570,driver<571 brand=vpc,driver>=570,driver<571 brand=vcs,driver>=570,driver<571 brand=vws,driver>=570,driver<571 brand=cloudgaming,driver>=570,driver<571 brand=unknown,driver>=575,driver<576 brand=grid,driver>=575,driver<576 brand=tesla,driver>=575,driver<576 brand=nvidia,driver>=575,driver<576 brand=quadro,driver>=575,driver<576 brand=quadrortx,driver>=575,driver<576 brand=nvidiartx,driver>=575,driver<576 brand=vapps,driver>=575,driver<576 brand=vpc,driver>=575,driver<576 brand=vcs,driver>=575,driver<576 brand=vws,driver>=575,driver<576 brand=cloudgaming,driver>=575,driver<576
PWD=/
NVIDIA_DRIVER_CAPABILITIES=compute,utility
NV_CUDA_CUDART_VERSION=13.0.96-1
HOME=/root
KUBERNETES_PORT_443_TCP=tcp://10.96.0.1:443
CUDA_VERSION=13.0.2
https_proxy=http://###.###.###.###:####
TERM=xterm
NO_PROXY=localhost,127.0.0.1,10.245.0.0/16,11.11.1.1,11.11.1.2,11.11.1.3,.svc,.cluster.local
SHLVL=1
NVARCH=x86_64
HTTPS_PROXY=http://###.###.###.###:####
KUBERNETES_PORT_443_TCP_PROTO=tcp
HTTP_PROXY=http://###.###.###.###:####
http_proxy=http://###.###.###.###:####
KUBERNETES_PORT_443_TCP_ADDR=10.96.0.1
LD_LIBRARY_PATH=/usr/local/nvidia/lib:/usr/local/nvidia/lib64:/usr/local/cuda/lib64
KUBERNETES_SERVICE_HOST=10.96.0.1
KUBERNETES_PORT=tcp://10.96.0.1:443
KUBERNETES_PORT_443_TCP_PORT=443
PATH=/usr/local/nvidia/bin:/usr/local/cuda/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
_=/usr/bin/env
-
```

## CPU Python Job

Create specification and schedule job:

```shell
cat <<'EOF' | kubectl apply -f -
apiVersion: v1
kind: Pod
metadata:
  name: test-cpu-python
spec:
  restartPolicy: Never
  containers:
  - name: nvidia-cuda
    image: nvidia/cuda:13.0.2-base-ubuntu22.04
    envFrom:
      - configMapRef:
          name: test-environment
    env:
        # So we can see incremental progress
      - name: PYTHONUNBUFFERED
        value: "1"
    command: ["/bin/sh", "-c"]
    args:
    - |
      set -e

      echo
      echo "-------- Printing environment --------"
      env
      echo
      echo "PYTHONUNBUFFERED=${PYTHONUNBUFFERED}"
      echo "-"
      echo

      echo
      echo "-------- Installing 'uv' --------"
      apt-get -y clean && apt-get -y update && apt-get -y install curl
      curl -LsSf https://astral.sh/uv/install.sh | sh
      . /root/.local/bin/env
      uv python install
      echo "-"
      echo

      echo
      echo "-------- Initializing project --------"
      uv init project
      cd project

      cat <<'EOFF' > main.py
      def main():
          print("Hello from a Python CPU job!")

      if __name__ == "__main__":
          main()

      EOFF
      echo "-"
      echo

      echo
      echo "-------- Creating project virtual environment --------"
      uv venv --clear
      echo "-"
      echo

      echo
      echo "-------- Activating project virtual environment --------"
      . .venv/bin/activate
      echo "-"
      echo

      echo
      echo "-------- Running job --------"
      uv run main.py
      echo "-"
      echo
    resources:
      limits:
        cpu: 1
EOF
```

Inspect the job:

```shell
kubectl get pods --all-namespaces -o wide
kubectl describe pod test-cpu-python
kubectl logs test-cpu-python -f
# kubectl delete pod test-cpu-python
```

Expected logs:

```text
-------- Printing environment --------
NVIDIA_VISIBLE_DEVICES=all
...
_=/usr/bin/env

PYTHONUNBUFFERED=1
-

-------- Installing 'uv' --------
Get:1 https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2204/x86_64  InRelease [1581 B]
...
Setting up curl (7.81.0-1ubuntu1.21) ...
...
downloading uv 0.9.18 x86_64-unknown-linux-gnu
...
installing to /root/.local/bin
  uv
  uvx
everything's installed!

To add $HOME/.local/bin to your PATH, either restart your shell or run:

    source $HOME/.local/bin/env (sh, bash, zsh)
    source $HOME/.local/bin/env.fish (fish)
Downloading cpython-3.14.2-linux-x86_64-gnu (download) (33.7MiB)
...
-

-------- Initializing project --------
Initialized project `project` at `/project`
-

-------- Creating project virtual environment --------
Using CPython 3.14.2
Creating virtual environment at: .venv
Activate with: source .venv/bin/activate
-

-------- Activating project virtual environment --------
-

-------- Running job --------
Hello from a Python CPU job!
-
```

## GPU Python Job

Create specification and schedule job. Note the `resources` section where we ask for a GPU node:

```shell
cat <<'EOF' | kubectl apply -f -
apiVersion: v1
kind: Pod
metadata:
  name: test-gpu-python
spec:
  restartPolicy: Never
  containers:
  - name: nvidia-cuda
    image: nvidia/cuda:13.0.2-base-ubuntu22.04
    envFrom:
      - configMapRef:
          name: test-environment
    env:
        # So we can see incremental progress
      - name: PYTHONUNBUFFERED
        value: "1"
        # Using older version of Pytorch with the latest Numpy and Python versions it supports
        # -> Required for cuda capability 6.0 which is required for P100 GPUs
      - name: PYTORCH_VERSION
        value: "1.13.1"
      - name: PYTHON_VERSION
        value: "3.11"
      - name: NUMPY_VERSION
        value: "1.26.4"
    command: ["/bin/sh", "-c"]
    args:
    - |
      set -e

      echo
      echo "-------- Printing environment --------"
      env
      echo
      echo "PYTHONUNBUFFERED=${PYTHONUNBUFFERED}"
      echo "PYTORCH_VERSION=${PYTORCH_VERSION}"
      echo "PYTHON_VERSION=${PYTHON_VERSION}"
      echo "NUMPY_VERSION=${NUMPY_VERSION}"
      echo "-"
      echo

      echo
      echo "-------- Installing 'uv' --------"
      apt-get -y clean && apt-get -y update && apt-get -y install curl
      curl -LsSf https://astral.sh/uv/install.sh | sh
      . /root/.local/bin/env
      uv python install ${PYTHON_VERSION}
      echo "-"
      echo

      echo
      echo "-------- Initializing project --------"
      uv init --python ${PYTHON_VERSION} project
      cd project

      cat <<'EOFFF' > main.py
      import time
      import torch

      def allocate(use_gpu_index, iterations=100, min_size=14336, max_size=18432, delay=0.1):
          device = torch.device(f"cuda:{use_gpu_index}")
          device_name = torch.cuda.get_device_name(use_gpu_index)
          print(f"GPU {use_gpu_index} testing: {device_name}")
          try:
              for i in range(iterations):
                  # Random size multiple of 8 between min_size and max_size
                  size = torch.randint(min_size // 8, max_size // 8 + 1, (1,)).item() * 8

                  # Using half precision to force use of tensor cores
                  x = torch.randn(size, size, device=device, dtype=torch.float16)

                  # Matrix multiplication
                  y = torch.mm(x, x.t())

                  # Non-linear activation function
                  z = torch.tanh(y)

                  # Summation along dimension 1
                  w = torch.sum(z, dim=1)

                  torch.cuda.synchronize()

                  print(f"Iteration {i}: GPU {use_gpu_index} tensor operations: SUCCESS")
                  print(f"Iteration {i}: GPU {use_gpu_index} tensor shape: {y.shape}")

                  if delay > 0:
                      time.sleep(delay)  # Optional delay between iterations to reduce heat or load
          except Exception as e:
              print(f"Iteration {i}: GPU {use_gpu_index} FAILED: {e}")

      def main():
          print("Hello from a Python GPU job!")

          print("-------- Start --------")
          use_gpu_index = 0
          allocate(use_gpu_index)
          print("-------- Finish --------")

      if __name__ == "__main__":
          main()

      EOFFF
      echo "-"
      echo

      echo
      echo "-------- Creating project virtual environment --------"
      uv venv --clear
      echo "-"
      echo

      echo
      echo "-------- Activating project virtual environment --------"
      . .venv/bin/activate
      echo "-"
      echo

      echo
      echo "-------- Installing dependencies --------"
      uv add "torch==${PYTORCH_VERSION}"
      uv add "numpy==${NUMPY_VERSION}"
      echo "-"
      echo

      echo
      echo "-------- Running GPU test --------"
      uv run main.py
      echo "-"
      echo
    resources:
      limits:
        nvidia.com/gpu: 1
EOF
```

Inspect the job:

```shell
kubectl get pods --all-namespaces -o wide
kubectl describe pod test-gpu-python
kubectl logs test-gpu-python -f
# kubectl delete pod test-gpu-python
```

Expected logs:

```text
-------- Printing environment --------
NVIDIA_VISIBLE_DEVICES=all
...
_=/usr/bin/env

PYTHONUNBUFFERED=1
PYTORCH_VERSION=1.13.1
PYTHON_VERSION=3.11
NUMPY_VERSION=1.26.4
-

-------- Installing 'uv' --------
Get:1 https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2204/x86_64  InRelease [1581 B]
...
Setting up curl (7.81.0-1ubuntu1.21) ...
...
downloading uv 0.9.18 x86_64-unknown-linux-gnu
...
installing to /root/.local/bin
  uv
  uvx
everything's installed!

To add $HOME/.local/bin to your PATH, either restart your shell or run:

    source $HOME/.local/bin/env (sh, bash, zsh)
    source $HOME/.local/bin/env.fish (fish)
Downloading cpython-3.14.2-linux-x86_64-gnu (download) (33.7MiB)
...
-

-------- Initializing project --------
Initialized project `project` at `/project`
-

-------- Creating project virtual environment --------
Using CPython 3.11.14
Creating virtual environment at: .venv
-

-------- Activating project virtual environment --------
-

-------- Installing dependencies --------
Resolved 9 packages in 478ms
...
Installed 8 packages in 267ms
 + nvidia-cublas-cu11==11.10.3.66
 + nvidia-cuda-nvrtc-cu11==11.7.99
 + nvidia-cuda-runtime-cu11==11.7.99
 + nvidia-cudnn-cu11==8.5.0.96
 + setuptools==80.9.0
 + torch==1.13.1
 + typing-extensions==4.15.0
 + wheel==0.45.1
...
Installed 1 package in 38ms
 + numpy==1.26.4
-

-------- Running GPU test --------
Hello from a Python GPU job!
-------- Start --------
GPU 0 testing: NVIDIA GeForce RTX 3060
Iteration 0: GPU 0 tensor operations: SUCCESS
Iteration 0: GPU 0 tensor shape: torch.Size([17040, 17040])
Iteration 1: GPU 0 tensor operations: SUCCESS
Iteration 1: GPU 0 tensor shape: torch.Size([17376, 17376])
...
Iteration 98: GPU 0 tensor operations: SUCCESS
Iteration 98: GPU 0 tensor shape: torch.Size([16968, 16968])
Iteration 99: GPU 0 tensor operations: SUCCESS
Iteration 99: GPU 0 tensor shape: torch.Size([14344, 14344])
-------- Finish --------
-
```

## Queues

### KAI Scheduler

Create hierarchical queues:

```shell
cat <<'EOF' | kubectl apply -f -
apiVersion: scheduling.run.ai/v2
kind: Queue
metadata:
  name: department-1
spec:
  resources:
    gpu:
      quota: -1  # Unlimited quota for demo
      limit: -1
    cpu:
      quota: -1
      limit: -1
---
apiVersion: scheduling.run.ai/v2
kind: Queue
metadata:
  name: team-a
spec:
  parentQueue: department-1
  resources:
    gpu:
      quota: -1
      limit: -1
    cpu:
      quota: -1
      limit: -1
EOF
```

Inspect the queues:

```shell
kubectl get queues -o wide
# NAME                   PRIORITY   PARENT                 CHILDREN            DISPLAYNAME
# default-parent-queue                                     ["default-queue"]
# default-queue                     default-parent-queue
# department-1                                             ["team-a"]
# team-a                            department-1

kubectl describe queue team-a
# Name:         team-a
# Namespace:
# Labels:       <none>
# Annotations:  <none>
# API Version:  scheduling.run.ai/v2
# Kind:         Queue
# Metadata:
#   Creation Timestamp:  2025-12-31T21:42:15Z
#   Generation:          1
#   Resource Version:    7971
#   UID:                 3cbd298b-f460-45c8-8f47-d85833c5c193
# Spec:
#   Parent Queue:  department-1
#   Resources:
#     Cpu:
#       Limit:  -1
#       Quota:  -1
#     Gpu:
#       Limit:  -1
#       Quota:  -1
# Events:       <none>
```

#### GPU Job

Create specification and schedule job:

```shell
cat <<'EOF' | kubectl apply -f -
apiVersion: v1
kind: Pod
metadata:
  name: test-queue-gpu
  labels:
    kai.scheduler/queue: team-a  # Assigns to child queue
spec:
  restartPolicy: Never
  containers:
  - name: test
    image: nvidia/cuda:13.0.2-base-ubuntu22.04
    command: ["sleep", "180"]
    resources:
      limits:
        nvidia.com/gpu: 1
EOF
```

Inspect the job:

```shell
kubectl get pods --all-namespaces -o wide
kubectl describe pod test-queue-gpu
kubectl logs test-queue-gpu

kubectl describe queue team-a
kubectl get pods -l kai.scheduler/queue=team-a --show-labels -o wide
# NAME             READY   STATUS    RESTARTS   AGE   IP            NODE  NOMINATED NODE   READINESS GATES   LABELS
# test-queue-gpu   1/1     Running   0          28s   10.244.2.20   wrk2  <none>           <none>            kai.scheduler/queue=team-a

# kubectl delete pod test-queue-gpu
```

### Kueue

Create the ResourceFlavor:

```shell
cat <<'EOF' | kubectl apply -f -
apiVersion: kueue.x-k8s.io/v1beta2
kind: ResourceFlavor
metadata:
  name: default-flavor  # Must exactly match ClusterQueue reference
EOF
```

Inspect the ResourceFlavors:

```shell
kubectl get resourceflavors --all-namespaces -o wide
# NAME             AGE
# default-flavor   13s
```

Create the ClusterQueue:

```shell
cat <<'EOF' | kubectl apply -f -
apiVersion: kueue.x-k8s.io/v1beta2
kind: ClusterQueue
metadata:
  name: adhoc-cluster-queue
spec:
  namespaceSelector: {} # match all
  resourceGroups:
  - coveredResources: ["cpu", "memory", "pods"]
    flavors:
    - name: default-flavor
      resources:
      - name: cpu
        nominalQuota: 9
      - name: memory
        nominalQuota: 36Gi
      - name: pods
        nominalQuota: 5
EOF
```

Inspect the ClusterQueues:

```shell
kubectl get clusterqueues --all-namespaces -o wide
# NAME                  COHORT   STRATEGY         PENDING WORKLOADS   ADMITTED WORKLOADS
# adhoc-cluster-queue            BestEffortFIFO   0                   0
# kubectl delete clusterqueues adhoc-cluster-queue
```

Create the LocalQueue:

```shell
cat <<'EOF' | kubectl apply -f -
apiVersion: kueue.x-k8s.io/v1beta2
kind: LocalQueue
metadata:
  namespace: default
  name: adhoc-local-queue
spec:
  clusterQueue: adhoc-cluster-queue
EOF
```

Inspect the LocalQueues:

```shell
kubectl get localqueues --all-namespaces -o wide
# NAMESPACE   NAME                CLUSTERQUEUE          PENDING WORKLOADS   ADMITTED WORKLOADS
# default     adhoc-local-queue   adhoc-cluster-queue   0                   0
# kubectl delete localqueues adhoc-local-queue
```

#### GPU Job

Create specification and schedule job:

```shell
cat <<'EOF' | kubectl create -f -
apiVersion: batch/v1
kind: Job
metadata:
  generateName: adhoc-job
  namespace: default
  labels:
    kueue.x-k8s.io/queue-name: adhoc-local-queue
spec:
  parallelism: 1
  completions: 1
  template:
    spec:
      containers:
      - name: dummy-job
        image: nvidia/cuda:13.0.2-base-ubuntu22.04
        command: [ "/bin/sh" ]
        args: [ "-c", "sleep 180" ]
        resources:
          requests:
            cpu: "1"
            memory: "200Mi"
      restartPolicy: Never
EOF
```

Inspect the workload:

```shell
kubectl -n default get workloads --all-namespaces -o wide
# NAME                       QUEUE               RESERVED IN           ADMITTED   FINISHED   AGE
# job-adhoc-jobfltbg-59ea6   adhoc-local-queue   adhoc-cluster-queue   True                  5s

kubectl -n default get jobs --all-namespaces -o wide
# NAME             STATUS    COMPLETIONS   DURATION   AGE
# adhoc-jobfltbg   Running   0/1           23s        23s

kubectl -n default describe workloads job-adhoc-jobfltbg-59ea6
# kubectl -n default delete workloads job-adhoc-jobfltbg-59ea6

kubectl -n default describe jobs adhoc-jobfltbg
# kubectl -n default delete jobs adhoc-jobfltbg
```

Output: `kubectl -n default describe workloads job-adhoc-jobfltbg-59ea6`:

```shell
Name:         job-adhoc-jobfltbg-59ea6
Namespace:    default
Labels:       kueue.x-k8s.io/job-uid=18b87461-24fb-4829-ad04-3cad301358fa
Annotations:  <none>
API Version:  kueue.x-k8s.io/v1beta2
Kind:         Workload
Metadata:
  Creation Timestamp:  2026-01-06T04:48:59Z
  Finalizers:
    kueue.x-k8s.io/resource-in-use
  Generation:  1
  Owner References:
    API Version:           batch/v1
    Block Owner Deletion:  true
    Controller:            true
    Kind:                  Job
    Name:                  adhoc-jobfltbg
    UID:                   18b87461-24fb-4829-ad04-3cad301358fa
  Resource Version:        10945
  UID:                     e1db6e8e-3d59-470e-894e-a2f35912a1aa
Spec:
  Active:  true
  Pod Sets:
    Count:  1
    Name:   main
    Template:
      Metadata:
        Labels:
          batch.kubernetes.io/job-name:  adhoc-jobfltbg
      Spec:
        Containers:
          Args:
            -c
            sleep 180
          Command:
            /bin/sh
          Image:              nvidia/cuda:13.0.2-base-ubuntu22.04
          Image Pull Policy:  IfNotPresent
          Name:               dummy-job
          Resources:
            Requests:
              Cpu:                     1
              Memory:                  200Mi
          Termination Message Path:    /dev/termination-log
          Termination Message Policy:  File
        Dns Policy:                    ClusterFirst
        Restart Policy:                Never
        Scheduler Name:                default-scheduler
        Security Context:
        Termination Grace Period Seconds:  30
    Topology Request:
      Pod Index Label:  batch.kubernetes.io/job-completion-index
  Priority:             0
  Queue Name:           adhoc-local-queue
Status:
  Admission:
    Cluster Queue:  adhoc-cluster-queue
    Pod Set Assignments:
      Count:  1
      Flavors:
        Cpu:     default-flavor
        Memory:  default-flavor
        Pods:    default-flavor
      Name:      main
      Resource Usage:
        Cpu:     1
        Memory:  200Mi
        Pods:    1
  Conditions:
    Last Transition Time:  2026-01-06T04:48:59Z
    Message:               Quota reserved in ClusterQueue adhoc-cluster-queue
    Observed Generation:   1
    Reason:                QuotaReserved
    Status:                True
    Type:                  QuotaReserved
    Last Transition Time:  2026-01-06T04:48:59Z
    Message:               The workload is admitted
    Observed Generation:   1
    Reason:                Admitted
    Status:                True
    Type:                  Admitted
Events:
  Type    Reason         Age    From             Message
  ----    ------         ----   ----             -------
  Normal  QuotaReserved  2m42s  kueue-admission  Quota reserved in ClusterQueue adhoc-cluster-queue, wait time since queued was 0s
  Normal  Admitted       2m42s  kueue-admission  Admitted by ClusterQueue adhoc-cluster-queue, wait time since reservation was 0s
```

Output: `kubectl -n default describe jobs adhoc-jobfltbg`:

```shell
Name:             adhoc-jobfltbg
Namespace:        default
Selector:         batch.kubernetes.io/controller-uid=18b87461-24fb-4829-ad04-3cad301358fa
Labels:           kueue.x-k8s.io/queue-name=adhoc-local-queue
Annotations:      <none>
Parallelism:      1
Completions:      1
Completion Mode:  NonIndexed
Suspend:          false
Backoff Limit:    6
Start Time:       Tue, 06 Jan 2026 04:48:59 +0000
Pods Statuses:    1 Active (1 Ready) / 0 Succeeded / 0 Failed
Pod Template:
  Labels:       batch.kubernetes.io/controller-uid=18b87461-24fb-4829-ad04-3cad301358fa
                batch.kubernetes.io/job-name=adhoc-jobfltbg
                controller-uid=18b87461-24fb-4829-ad04-3cad301358fa
                job-name=adhoc-jobfltbg
                kueue.x-k8s.io/podset=main
  Annotations:  kueue.x-k8s.io/workload: job-adhoc-jobfltbg-59ea6
  Containers:
   dummy-job:
    Image:      nvidia/cuda:13.0.2-base-ubuntu22.04
    Port:       <none>
    Host Port:  <none>
    Command:
      /bin/sh
    Args:
      -c
      sleep 180
    Requests:
      cpu:         1
      memory:      200Mi
    Environment:   <none>
    Mounts:        <none>
  Volumes:         <none>
  Node-Selectors:  <none>
  Tolerations:     <none>
Events:
  Type    Reason            Age    From                        Message
  ----    ------            ----   ----                        -------
  Normal  Suspended         2m59s  job-controller              Job suspended
  Normal  CreatedWorkload   2m59s  batch/job-kueue-controller  Created Workload: default/job-adhoc-jobfltbg-59ea6
  Normal  Started           2m59s  batch/job-kueue-controller  Admitted by clusterQueue adhoc-cluster-queue
  Normal  SuccessfulCreate  2m59s  job-controller              Created pod: adhoc-jobfltbg-mjzv5
  Normal  Resumed           2m59s  job-controller              Job resumed
```
