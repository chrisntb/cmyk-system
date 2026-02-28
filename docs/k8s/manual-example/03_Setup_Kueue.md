# K8s Cluster - Manual Example - Setup Kueue

> In order to setup Kueue you must first have completed the Ad Hoc Cluster setup tasks, see [Setup](02_Setup.md)

These instruction were adapted from the official installation instructions:

- `https://kueue.sigs.k8s.io/docs/installation/`

In short the setup installs Kueue using its K8s manifest, however there is also a Helm chart available.

## On Controller Node

Create setup script: setup-kueue.sh

```shell
cat <<'EOF' > setup-kueue.sh
#!/bin/bash
set -e
# ********
#
# USAGE
# setup-kueue.sh
#
# ********

# See https://github.com/kubernetes-sigs/kueue/releases
export KUEUE_VERSION="0.15.2"

echo
echo "-------- SETTING UP --------"
echo "KUEUE_VERSION=${KUEUE_VERSION}"
echo "-"
echo

echo
echo "-------- SETTING UP - Install --------"
kubectl apply --server-side -f https://github.com/kubernetes-sigs/kueue/releases/download/v${KUEUE_VERSION}/manifests.yaml
echo "-"
echo

echo
echo "-------- COMPLETE --------"
echo

EOF
```

Run setup script:

```shell
chmod 700 setup-kueue.sh

# Note running as the 'ubuntu' user
./setup-kueue.sh 2>&1 | tee -a setup-kueue.sh.log
```

Output:

```shell
namespace/kueue-system serverside-applied
customresourcedefinition.apiextensions.k8s.io/admissionchecks.kueue.x-k8s.io serverside-applied
customresourcedefinition.apiextensions.k8s.io/clusterqueues.kueue.x-k8s.io serverside-applied
customresourcedefinition.apiextensions.k8s.io/cohorts.kueue.x-k8s.io serverside-applied
customresourcedefinition.apiextensions.k8s.io/localqueues.kueue.x-k8s.io serverside-applied
customresourcedefinition.apiextensions.k8s.io/multikueueclusters.kueue.x-k8s.io serverside-applied
customresourcedefinition.apiextensions.k8s.io/multikueueconfigs.kueue.x-k8s.io serverside-applied
customresourcedefinition.apiextensions.k8s.io/provisioningrequestconfigs.kueue.x-k8s.io serverside-applied
customresourcedefinition.apiextensions.k8s.io/resourceflavors.kueue.x-k8s.io serverside-applied
customresourcedefinition.apiextensions.k8s.io/topologies.kueue.x-k8s.io serverside-applied
customresourcedefinition.apiextensions.k8s.io/workloadpriorityclasses.kueue.x-k8s.io serverside-applied
customresourcedefinition.apiextensions.k8s.io/workloads.kueue.x-k8s.io serverside-applied
serviceaccount/kueue-controller-manager serverside-applied
role.rbac.authorization.k8s.io/kueue-leader-election-role serverside-applied
role.rbac.authorization.k8s.io/kueue-manager-clusterprofiles-role serverside-applied
role.rbac.authorization.k8s.io/kueue-manager-secrets-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-batch-admin-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-batch-user-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-clusterqueue-editor-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-clusterqueue-viewer-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-cohort-editor-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-cohort-viewer-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-jaxjob-editor-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-jaxjob-viewer-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-job-editor-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-job-viewer-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-jobset-editor-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-jobset-viewer-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-localqueue-editor-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-localqueue-viewer-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-manager-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-metrics-auth-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-metrics-reader serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-mpijob-editor-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-mpijob-viewer-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-paddlejob-editor-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-paddlejob-viewer-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-pending-workloads-cq-viewer-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-pending-workloads-lq-viewer-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-pytorchjob-editor-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-pytorchjob-viewer-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-raycluster-editor-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-raycluster-viewer-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-rayjob-editor-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-rayjob-viewer-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-resourceflavor-editor-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-resourceflavor-viewer-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-tfjob-editor-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-tfjob-viewer-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-topology-editor-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-topology-viewer-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-trainjob-editor-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-trainjob-viewer-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-workload-editor-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-workload-viewer-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-xgboostjob-editor-role serverside-applied
clusterrole.rbac.authorization.k8s.io/kueue-xgboostjob-viewer-role serverside-applied
rolebinding.rbac.authorization.k8s.io/kueue-visibility-server-auth-reader serverside-applied
rolebinding.rbac.authorization.k8s.io/kueue-leader-election-rolebinding serverside-applied
rolebinding.rbac.authorization.k8s.io/kueue-manager-clusterprofiles-rolebinding serverside-applied
rolebinding.rbac.authorization.k8s.io/kueue-manager-secrets-rolebinding serverside-applied
clusterrolebinding.rbac.authorization.k8s.io/kueue-manager-rolebinding serverside-applied
clusterrolebinding.rbac.authorization.k8s.io/kueue-metrics-auth-rolebinding serverside-applied
configmap/kueue-manager-config serverside-applied
secret/kueue-webhook-server-cert serverside-applied
service/kueue-controller-manager-metrics-service serverside-applied
service/kueue-visibility-server serverside-applied
service/kueue-webhook-service serverside-applied
deployment.apps/kueue-controller-manager serverside-applied
apiservice.apiregistration.k8s.io/v1beta1.visibility.kueue.x-k8s.io serverside-applied
apiservice.apiregistration.k8s.io/v1beta2.visibility.kueue.x-k8s.io serverside-applied
mutatingwebhookconfiguration.admissionregistration.k8s.io/kueue-mutating-webhook-configuration serverside-applied
validatingwebhookconfiguration.admissionregistration.k8s.io/kueue-validating-webhook-configuration serverside-applied
```

Checks:

```shell
kubectl get pods --all-namespaces -o wide
# NAMESPACE      NAME                                        READY   STATUS    RESTARTS   AGE     IP              NODE        NOMINATED NODE   READINESS GATES
# kube-flannel   kube-flannel-ds-4d4zr                       1/1     Running   0          8m18s   11.11.1.3       wrk2        <none>           <none>
# kube-flannel   kube-flannel-ds-bks98                       1/1     Running   0          8m24s   11.11.1.2       wrk1        <none>           <none>
# kube-flannel   kube-flannel-ds-tng5c                       1/1     Running   0          9m40s   11.11.1.1       ctl         <none>           <none>
# kube-system    coredns-7d764666f9-9qwww                    1/1     Running   0          10m     10.244.0.2      ctl         <none>           <none>
# kube-system    coredns-7d764666f9-j479m                    1/1     Running   0          10m     10.244.0.3      ctl         <none>           <none>
# kube-system    etcd-ctl                                    1/1     Running   0          11m     11.11.1.1       ctl         <none>           <none>
# kube-system    kube-apiserver-ctl                          1/1     Running   0          11m     11.11.1.1       ctl         <none>           <none>
# kube-system    kube-controller-manager-ctl                 1/1     Running   0          11m     11.11.1.1       ctl         <none>           <none>
# kube-system    kube-proxy-clnjb                            1/1     Running   0          8m24s   11.11.1.2       wrk1        <none>           <none>
# kube-system    kube-proxy-hrcm8                            1/1     Running   0          8m18s   11.11.1.3       wrk2        <none>           <none>
# kube-system    kube-proxy-zxp8c                            1/1     Running   0          10m     11.11.1.1       ctl         <none>           <none>
# kube-system    kube-scheduler-ctl                          1/1     Running   0          11m     11.11.1.1       ctl         <none>           <none>
# kueue-system   kueue-controller-manager-595dcf7455-pkx94   1/1     Running   0          4m51s   10.244.2.2      wrk2        <none>           <none>

kkubectl get svc --all-namespaces
# NAMESPACE      NAME                                       TYPE        CLUSTER-IP      EXTERNAL-IP   PORT(S)                  AGE
# default        kubernetes                                 ClusterIP   10.96.0.1       <none>        443/TCP                  8m33s
# kube-system    kube-dns                                   ClusterIP   10.96.0.10      <none>        53/UDP,53/TCP,9153/TCP   8m32s
# kueue-system   kueue-controller-manager-metrics-service   ClusterIP   10.109.79.202   <none>        8443/TCP                 2m18s
# kueue-system   kueue-visibility-server                    ClusterIP   10.102.221.16   <none>        443/TCP                  2m18s
# kueue-system   kueue-webhook-service                      ClusterIP   10.104.145.88   <none>        443/TCP                  2m18s
kubectl logs -n kueue-system kueue-controller-manager-595dcf7455-pkx94
```
