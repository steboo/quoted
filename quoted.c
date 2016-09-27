#define _POSIX_C_SOURCE 200809L
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#define LISTEN_BACKLOG 128
#define BUF_LEN 1024 * 1024

int quoted(char *port, bool tcp, bool udp) {
  int sockfd, client_sockfd;
  struct addrinfo hints, *res;
  struct sockaddr_storage client_addr;
  char *quote = "You will never live if you are looking for the meaning of life. -- Albert Camus";
  int quote_len, bytes_sent;
  socklen_t sin_size;

  // Use getaddrinfo to easily support both IPv4 and IPv6
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;

  if (tcp) {
    hints.ai_socktype = SOCK_STREAM;
  } else if (udp) {
    hints.ai_socktype = SOCK_DGRAM;
  }

  hints.ai_flags = AI_PASSIVE;

  if (getaddrinfo(NULL, port, &hints, &res) == -1) {
    perror("getaddrinfo");
    exit(1);
  }

  sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  if (sockfd == -1) {
    perror("socket");
    exit(1);
  }

  if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
    perror("bind");
    close(sockfd);
    exit(1);
  }

  freeaddrinfo(res);

  if (tcp) {
    if (listen(sockfd, LISTEN_BACKLOG) == -1) {
      perror("listen");
      close(sockfd);
      exit(1);
    }
  }

  while (true) {
    char buf[BUF_LEN];
    sin_size = sizeof client_addr;
    if (tcp) {
      client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);

      if (client_sockfd == -1) {
        perror("accept");
        close(sockfd);
        exit(1);
      }
    } else if (udp) {
      int bytes_recv = recvfrom(sockfd, buf, BUF_LEN,  0, (struct sockaddr *)&client_addr, &sin_size);

      if (bytes_recv == -1) {
        perror("recvfrom");
        close(sockfd);
        exit(1);
      }
    }

    quote_len = strlen(quote);
    if (tcp) {
      bytes_sent = send(client_sockfd, quote, quote_len, 0);

      if (bytes_sent == -1) {
        perror("send");
        close(client_sockfd);
        close(sockfd);
        exit(1);
      }
    } else if (udp) {
      bytes_sent = sendto(sockfd, quote, quote_len, 0, (struct sockaddr *)&client_addr, sin_size);

      if (bytes_sent == -1) {
        perror("sendto");
        close(sockfd);
        exit(1);
      }
    }

    if (tcp) {
      close(client_sockfd);
    }
  }

  close(sockfd);

  return 0;
}

int main(int argc, char* argv[]) {
  bool default_use_tcp, explicit_use_tcp, use_udp;
  default_use_tcp = true;
  explicit_use_tcp = false;
  use_udp = false;
  char *port = "17"; // Port 17 is specified in RFC 865
  int i, opt, port_len;

  opterr = 0;

  while ((opt = getopt(argc, argv, "p:tu")) != -1) {
    switch (opt) {
      case 'p':
        if (optarg == NULL) {
          fprintf(stderr, "Error: PORT must be an integer\n");
          fprintf(stderr, "Usage: %s [-tu] [-p PORT]\n", argv[0]);
          return EXIT_FAILURE;
        }

        port = optarg;
        port_len = strlen(port);

        for (i = 0; i < port_len; i++) {
          if (!isdigit(optarg[i])) {
            fprintf(stderr, "Error: PORT must be an integer\n");
            fprintf(stderr, "Usage: %s [-tu] [-p PORT]\n", argv[0]);
            return EXIT_FAILURE;
          }
        }

        int port_num = atoi(optarg);

        if (port_num < 0 || port_num > 65535) {
          fprintf(stderr, "Error: PORT must be between 0 and 65535\n");
          fprintf(stderr, "Usage: %s [-tu] [-p PORT]\n", argv[0]);
          return EXIT_FAILURE;
        }
        break;
      case 't':
        explicit_use_tcp = true;
        break;
      case 'u':
        use_udp = true;
        default_use_tcp = false;
        break;
      default:
        fprintf(stderr, "Usage: %s [-tu] [-p PORT]\n", argv[0]);
        return EXIT_FAILURE;
    }
  }

  quoted(port, default_use_tcp || explicit_use_tcp, use_udp);
  return EXIT_SUCCESS;
}
