# Generating SSH fingerprints
This guide shows how to generate TLS/SSL SHA1 fingerprints that are used in
the program to validate the TLS/SSL certificates send by the server. In
the `main.cpp` file there are 2 variables that contains strings with SHA1
fingerprints. The upper one `apiInf1iGaFingerprint` is used to validate
the api.inf1i.ga certificate and the lower one `mqttInf1iGaFingerprint`
to validate mqtt.inf1i.ga.
```CPP
PROGMEM const char *apiInf1iGaFingerprint = "ED 1D 2C E2 41 5D 35 81 F6 15 DB A1 C8 0B 19 71 32 67 8B 46";
PROGMEM const char *mqttInf1iGaFingerprint = "A6 E4 A9 8C 92 B3 8D 81 73 CE 5B 33 33 F5 A3 7A 1B 87 E2 F3";
```

## Generating fingerprints using openssh
To obtain an certain fingerprint from an TLS/SSL certificate we can use
the openssh commandline tool. Open an terminal type:
```bash
 openssl s_client -servername {subdomain}.inf1i.ga -connect 146.185.164.96:8883 < /dev/null 2>/dev/null | openssl x509 -fingerprint -noout -in /dev/stdin | tr : " "
```
The output should look like this:
```bash
SHA1 Fingerprint=A6 E4 A9 8C 92 B3 8D 81 73 CE 5B 33 33 F5 A3 7A 1B 87 E2 F3
```
To add the fingerprint to the code copy everything after `SHA1 Fingerprint=` to
the fingerprint variable. You should now be able to validate the certificate so
the messages are send securely over the internet.