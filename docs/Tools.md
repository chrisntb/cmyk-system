# Tools

This project requires the development tools documented here.

## Canonical Multipass

See `https://documentation.ubuntu.com/multipass/latest/tutorial/`.

## UV / Python

```shell
curl -LsSf https://astral.sh/uv/install.sh | sh

uv python install 3.14
```

## VS Code

Recommended VS Code settings are available when using the workpace `cmyk-system.code-workspace`.

### Extensions

> Check extension specific documentation since there are often post-install instructions.

- Claude Code for VS Code
  - `https://marketplace.visualstudio.com/items?itemName=anthropic.claude-code`

- Ansible
  - `https://marketplace.visualstudio.com/items?itemName=redhat.ansible`

- Even Better TOML
  - `https://marketplace.visualstudio.com/items?itemName=tamasfe.even-better-toml`

Python:

- Python
  - `https://marketplace.visualstudio.com/items?itemName=ms-python.python`
- Ruff
  - `https://marketplace.visualstudio.com/items?itemName=charliermarsh.ruff`
- ty
  - `https://marketplace.visualstudio.com/items?itemName=astral-sh.ty`

Documentation:

- Markdownlint
  - `https://marketplace.visualstudio.com/items?itemName=DavidAnson.vscode-markdownlint`
- Markdown Preview Mermaid Support
  - `https://marketplace.visualstudio.com/items?itemName=bierner.markdown-mermaid`
- Mermaid Markdown Syntax Highlighting
  - `https://marketplace.visualstudio.com/items?itemName=bpruitt-goddard.mermaid-markdown-syntax-highlighting`

### Settings

Recommended additional `user settings`:

```json
{
  //...
  "telemetry.telemetryLevel": "off",
  //...
  "window.nativeTabs": true,
  "window.openFoldersInNewWindow": "on",
  //...
}
```
