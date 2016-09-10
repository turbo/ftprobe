# ftprobe

Barebones command-line utility to check if anonymous can successfully login on a given FTP server. Compile with whatever and run

```
ftprobe 1.1.1.1
ftprobe ftp.example.com
```

It will print `OK\n` if anon is allowed and nothing if not. Use something like GNU `parallel` to perform mass-scans. ftprobe puts *much* less strain on memory, network and CPU than lftp and even ftp. A consumer CPU should handle a few hundred parallel probes with no problems.
