[![StackForge](https://img.shields.io/badge/StackForge-Stage%20II-ffb600.svg?style=for-the-badge)](#stackforge)

[![Authors](https://img.shields.io/badge/Authors-A.Korman%20J.Nolasco%20N.Novotny-blue.svg?style=for-the-badge)](LICENSE.md)

# StackForge

A StackForge compiler developed in C with Flex and Bison by A.Korman, J.Nolasco, and N.Novotny.

* [Requirements](#requirements)
* [Configuration](#configuration)
* [Commands](#commands)
* [CI/CD](#cicd)
* [Recommended Extensions](#recommended-extensions)

## Requirements

* [Docker v28.3.2](https://www.docker.com/)

## Configuration

Set the following environment variables to control and configure the behaviour of the application:

| Name                  | Default | Description                                                                                                                                                           |
| :-------------------- | :-----: | :-------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `ENVIRONMENT`         | `Local` | The active environment name. The available environments are: `Local`, `Development` and `Production`.                                                                 |
| `LOG_IGNORED_LEXEMES` | `true`  | When `true`, logs all of the ignored lexemes found with Flex at `DEBUGGING` level. To remove those logs from the console output set it to `false`.                    |
| `LOGGING_LEVEL`       | `ALL`   | The minimum level to log in the console output. From lower to higher, the available levels are: `ALL`, `DEBUGGING`, `INFORMATION`, `WARNING`, `ERROR` and `CRITICAL`. |

_Docker Compose_ can read the variables from an `.env` file too (see `compose.yaml` file).

## Commands

### Start

Rises an ephemeral container, ready to start development:

```bash
docker compose run --rm compiler
```

### Build

Builds or rebuilds the entire compiler:

```bash
src/main/bash/build.sh
```

### Run

Compiles a program:

```bash
src/main/bash/run.sh <program>
```

where `<program>` is the path to the file that represents its entry-point.

### Test

Executes the Stage II frontend unit tests under `src/test/c/accept` and
`src/test/c/reject`:

```bash
src/main/bash/test.sh
```

The fixtures under `src/test/c/reject-stage3` document semantic rejection cases
that require the Stage III backend/semantic-analysis phase, such as duplicate
service or network names, missing service references, invalid exposed ports,
exposing internal services (e.g. a `database`), cross-network connections
without an app-level network link, or mounts that reference undeclared volumes
or services. They are intentionally not executed by the Stage II test script
because the current deliverable only validates lexical and syntactic analysis
and AST construction.

### Language

A StackForge program declares an `app` composed of one or more `network`
blocks (each network maps to an isolated Docker Compose network) plus
network-to-network links. Inside a network, services are declared with a
topological role — `proxy` (reverse-proxy, meant to be exposed), `service`
(generic internal application), `static` (static content server), `database`,
and `cache` (internal by default) — and can be exposed, connected with the
`->` operator, and given volumes or host mounts:

```
app Shop {
    network edge {
        proxy gateway using "nginx:1.25";
        static assets using "nginx:1.25";
        expose gateway on 80;
        gateway -> assets;
    }
    network backend {
        service api using "node:20";
        database db using "postgres:16";
        cache sessions using "redis:7";
        api -> db;
        api -> sessions;
        volume pgdata;
        mount pgdata on db at "/var/lib/postgresql/data";
        mount "./src" on api at "/app/src";
    }
    edge -> backend;
}
```

Each role implies a distinct network topology in the generated artifact. In
Stage III, the compiler will derive a `docker-compose.yml` (networks,
memberships, dependencies, volumes, and published ports) from a valid
specification, along with a very basic per-service scaffolding (folders and
Dockerfile stubs for known images).

### Stop

Logout, destroy the ephemeral containers and shutdowns the cluster:

```bash
exit
docker compose down
```

### Docker

| Command                                 | Description                                             |
| :-------------------------------------- | :------------------------------------------------------ |
| `docker builder prune --all`            | Removes all builds and complete build cache.            |
| `docker compose --progress=plain build` | Forces a build or rebuild of the images in the cluster. |
| `docker image prune`                    | Removes all of the dangling images from Docker.         |
| `docker network prune`                  | Removes unused networks from Docker.                    |
| `docker volume prune`                   | Removes unused volumes from Docker.                     |

## CI/CD

To trigger an automatic integration on every push or PR (_Pull Request_), you must activate _GitHub Actions_ in the _Settings_ tab. Use the following configuration:

| Key                                                        | Value                                               |
| :--------------------------------------------------------- | :-------------------------------------------------- |
| `Actions permissions`                                      | `Allow all actions and reusable workflows`          |
| `Allow GitHub Actions to create and approve pull requests` | `false`                                             |
| `Artifact and log retention`                               | `30 days`                                           |
| `Fork pull request workflows from outside collaborators`   | `Require approval for all outside collaborators`    |
| `Workflow permissions`                                     | `Read repository contents and packages permissions` |

## Recommended Extensions

* [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
* [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)
* [Yash](https://marketplace.visualstudio.com/items?itemName=daohong-emilio.yash)
