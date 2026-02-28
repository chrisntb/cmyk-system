Host ctl
    ProxyJump j
    HostName 10.10.228.153
    User ubuntu
    IdentityFile ~/.ssh/id_ed25519
    IdentitiesOnly yes

Host wrk-hpc-1
    ProxyJump j
    HostName 10.10.228.148
    User ubuntu
    IdentityFile ~/.ssh/id_ed25519
    IdentitiesOnly yes

Host wrk-hpc-2
    ProxyJump j
    HostName 10.10.228.154
    User ubuntu
    IdentityFile ~/.ssh/id_ed25519
    IdentitiesOnly yes

#Host wrk-mgt-1
#    ProxyJump j
#    HostName ###.###.###.###
#    User ubuntu
#    IdentityFile ~/.ssh/id_ed25519
#    IdentitiesOnly yes
