# K8s Cluster Example - Setup KAI Scheduler

> In order to setup the KAI Scheduler you must first have completed the Ad Hoc Cluster setup tasks, see [Setup](02_Setup.md)

These instruction were adapted from the official installation instructions:

- `https://github.com/NVIDIA/KAI-Scheduler?tab=readme-ov-file#installation`
- `https://github.com/NVIDIA/KAI-Scheduler/blob/main/docs/quickstart/README.md`

In short the setup installs the KAI Scheduler using its Helm chart.

## On Controller Node

Create setup script: setup-kai-scheduler.sh

```shell
cat <<'EOF' > setup-kai-scheduler.sh
#!/bin/bash
set -e
# ********
#
# USAGE
# setup-kai-scheduler.sh
#
# ********

# See https://github.com/NVIDIA/KAI-Scheduler/releases
export KAI_SCHEDULER_VERSION="0.12.4"

echo
echo "-------- SETTING UP --------"
echo "KAI_SCHEDULER_VERSION=${KAI_SCHEDULER_VERSION}"
echo "-"
echo

echo
echo "-------- SETTING UP - Install --------"
helm install kai-scheduler oci://ghcr.io/nvidia/kai-scheduler/kai-scheduler --namespace kai-scheduler --create-namespace --version v${KAI_SCHEDULER_VERSION}
echo "-"
echo

echo
echo "-------- COMPLETE --------"
echo

EOF
```

Run setup script:

```shell
chmod 700 setup-kai-scheduler.sh

# Note running as the 'ubuntu' user
./setup-kai-scheduler.sh 2>&1 | tee -a setup-kai-scheduler.sh.log
```

Output:

```shell
Pulled: ghcr.io/nvidia/kai-scheduler/kai-scheduler:v0.12.3
Digest: sha256:cb1e618314cb499a03d47402ee1b1b52316b057d71d87d6ad5a9db1d59e0f8ca
NAME: kai-scheduler
LAST DEPLOYED: Wed Dec 31 21:09:09 2025
NAMESPACE: kai-scheduler
STATUS: deployed
REVISION: 1
DESCRIPTION: Install complete
TEST SUITE: None
```

Checks:

```shell
helm list -A
# NAME          NAMESPACE     REVISION UPDATED                                 STATUS   CHART                 APP VERSION
# kai-scheduler kai-scheduler 1        2026-01-09 03:29:28.798776451 +0000 UTC deployed kai-scheduler-v0.12.4 v0.12.4

helm get values kai-scheduler -n kai-scheduler -o yaml
# null

kubectl get pods --all-namespaces -o wide
# NAMESPACE       NAME                                   READY   STATUS    RESTARTS   AGE    IP              NODE        NOMINATED NODE   READINESS GATES
# kai-scheduler   admission-5dbf884b4d-mxsn5             1/1     Running   0          117s   10.244.2.6      wrk2        <none>           <none>
# kai-scheduler   binder-6c88878778-m9nfd                1/1     Running   0          117s   10.244.2.8      wrk2        <none>           <none>
# kai-scheduler   kai-operator-f69885989-9g4zs           1/1     Running   0          2m5s   10.244.2.4      wrk2        <none>           <none>
# kai-scheduler   kai-scheduler-default-bcd44487-25vjq   1/1     Running   0          117s   10.244.1.2      wrk2        <none>           <none>
# kai-scheduler   pod-grouper-d55f59cdb-xhl5n            1/1     Running   0          117s   10.244.2.9      wrk2        <none>           <none>
# kai-scheduler   podgroup-controller-6f5d4958b-zc4pd    1/1     Running   0          117s   10.244.2.7      wrk2        <none>           <none>
# kai-scheduler   queue-controller-6976b95b58-ct5hm      1/1     Running   0          117s   10.244.2.5      wrk2        <none>           <none>
# kube-flannel    kube-flannel-ds-dnb6r                  1/1     Running   0          21m    11.11.1.1       ctl         <none>           <none>
# kube-flannel    kube-flannel-ds-w7cg2                  1/1     Running   0          20m    11.11.1.3       wrk2        <none>           <none>
# kube-flannel    kube-flannel-ds-zj2cf                  1/1     Running   0          20m    11.11.1.2       wrk1        <none>           <none>
# kube-system     coredns-7d764666f9-9rflh               1/1     Running   0          24m    10.244.0.3      ctl         <none>           <none>
# kube-system     coredns-7d764666f9-kz7k2               1/1     Running   0          24m    10.244.0.2      ctl         <none>           <none>
# kube-system     etcd-ctl                               1/1     Running   0          24m    11.11.1.1       ctl         <none>           <none>
# kube-system     kube-apiserver-ctl                     1/1     Running   0          24m    11.11.1.1       ctl         <none>           <none>
# kube-system     kube-controller-manager-ctl            1/1     Running   0          24m    11.11.1.1       ctl         <none>           <none>
# kube-system     kube-proxy-7dzww                       1/1     Running   0          24m    11.11.1.1       ctl         <none>           <none>
# kube-system     kube-proxy-9mtk5                       1/1     Running   0          20m    11.11.1.3       wrk2        <none>           <none>
# kube-system     kube-proxy-c74nc                       1/1     Running   0          20m    11.11.1.2       wrk1        <none>           <none>
# kube-system     kube-scheduler-ctl                     1/1     Running   0          24m    11.11.1.1       ctl         <none>           <none>

kubectl logs -n kai-scheduler kai-operator-f69885989-9g4zs
kubectl logs -n kai-scheduler kai-scheduler-default-bcd44487-25vjq
```
