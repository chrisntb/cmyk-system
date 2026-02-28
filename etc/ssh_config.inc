Host ctl
    HostName 192.168.64.25
    User ubuntu
    IdentityFile ~/.ssh/id_ed25519
    IdentitiesOnly yes

Host wrk-hpc-1
    HostName 192.168.64.26
    User ubuntu
    IdentityFile ~/.ssh/id_ed25519
    IdentitiesOnly yes

Host wrk-hpc-2
    HostName 192.168.64.27
    User ubuntu
    IdentityFile ~/.ssh/id_ed25519
    IdentitiesOnly yes

#Host wrk-mgt-1
#    HostName ###.###.###.###
#    User ubuntu
#    IdentityFile ~/.ssh/id_ed25519
#    IdentitiesOnly yes
