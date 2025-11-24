# 10-10-Project
*Enclave (Trusted Execution Environment, TEE): A protected region (TCB, Trusted Computing Base) created within the CPU to safeguard sensitive code/data from external attacks (e.g., OS, malicious software).

*Communication: Secure communication between the Enclave interior and the exterior (Host OS) is achieved through E-Call (External → Internal) and O-Call (Internal → External).

*Key Enclave Functions
-Attestation: A mechanism to verify the trustworthiness of another Enclave/platform (Local or Remote).
-Sealing: The ability to encrypt/decrypt data for storage, ensuring it can only be accessed by the Enclave itself.

*Application and Tools
-Usage Example: An application for comparing user location data by anonymizing it and safely comparing it with confirmed patient routes inside the Enclave, ensuring no personal data leakage.
-Graphene/Gramine: An OS designed to automate the development of Enclave-enabled applications.
