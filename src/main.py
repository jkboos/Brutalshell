import vllm
import os

def main():
    default_host = "http://100.68.65.78:8887"
    host_env = os.getenv("VLLM_SERVER_URL")
    if host_env:
        host = host_env
    else:
        host = default_host
    stats = vllm.completions("Hi there", host)
    print(stats)

if __name__ == "__main__":
    main()
