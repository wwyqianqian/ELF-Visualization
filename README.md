# ELF-Visualization

## 更新信息（2025-12-16）
- 当前电脑环境：
  - macOS（Apple Silicon）
  - 全局 Node `v23.7.0`
  - 通过 `nvm` 在项目内使用 Node `v16.20.2`（见 `.nvmrc`）
- 变更说明：
  - 旧 README 要求 Node 14，在 Apple Silicon 上通过 `nvm install 14` 源码编译 V8 会失败（枚举范围相关编译错误），因此改为在本机以 Node 16 运行并验证通过
  - 删除了历史 `package-lock.json` 中指向 `registry.npm.taobao.org` 的锁定，避免证书过期导致 `CERT_HAS_EXPIRED`
  - 统一使用官方 npm registry：`https://registry.npmjs.org/`
  - 写入 `.nvmrc` 并设为 `16`，进入项目目录后可自动 `nvm use`
  - 保留旧版说明于 `README_legacy.md`

## 项目简介
- 使用 C 编写的 ELF 解析器（`elf_parser.c`），解析 ELF Header、Section Headers、Program Headers、Symbol Table、Relocations 等
- 使用 Next.js + Material-UI 构建前端界面，通过 API 路由调用本地解析器并将输出流式展示
- 运行地址：`http://localhost:3000/`

## 环境准备
- 安装 nvm（如未安装）：
  ```
  curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.7/install.sh | bash
  export NVM_DIR="$HOME/.nvm"
  . "$NVM_DIR/nvm.sh"
  ```
- 切换 Node 版本：
  ```
  nvm use
  node -v   # 应显示 v16.x
  ```
- 如需安装 Node 16（首次）：
  ```
  nvm install 16
  nvm use 16
  ```

## 安装与启动
1) 安装依赖（使用官方 registry，若你本地默认不是 npmjs，可先切换）
   ```
   npm config set registry https://registry.npmjs.org/
   npm install
   ```
2) 编译解析器
   ```
   gcc elf_parser.c -o elf_parser
   ```
3) 启动开发服务器
   ```
   npm run dev
   ```
   打开 `http://localhost:3000/`

## 使用说明
- 将待解析的 ELF 文件放入 `binaries/` 目录（页面会自动列出）
- 在左侧选择操作（`-h/-S/-l/-s/-r`）与文件，点击 Execute，输出将显示在右侧

## 关于 Node 14 的兼容方案（可选）
- 若必须严格使用 Node 14：
  - 在 Apple Silicon 上使用 Rosetta 终端后安装：
    ```
    arch -x86_64 zsh
    export NVM_DIR="$HOME/.nvm"; . "$NVM_DIR/nvm.sh"
    nvm install 14 && nvm use 14
    ```
  - 或使用 Docker：
    ```
    docker run -it -v "$PWD":/app -w /app -p 3000:3000 node:14 bash
    npm install
    gcc elf_parser.c -o elf_parser
    npm run dev
    ```
  - 注意需避免旧的 taobao 源以及过期证书

## 目录纠正
- 旧 README 中的 `cd elfviewer` 已过时；当前项目根目录即可运行，无需进入子目录

## License
[MIT](https://github.com/wwyqianqian/ELF-Visualization/blob/main/LICENSE) © wwyqianqian
