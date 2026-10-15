# Preparing this package for Git

The package has a root .gitignore and .gitattributes. The board-tested bitstream and XSA are intentionally included; model weights and generated Vivado/Vitis build caches are excluded.

From this directory, verify the snapshot before staging:

    python tools/verify_release.py
    git status --short

If you want this folder to become a standalone local repository, initialize it here, review the staged file list, then commit it:

    git init
    git add --all
    git status --short
    git commit -m "Add Qwen V2 FPGA board-tested package"

After creating an empty remote repository, connect its URL and push the branch according to your Git host. No remote URL is configured in this workspace, so this package has not been pushed.

The largest individual checked-in artifacts are the bitstream and XSA; both are below 50 MB. If your Git host has a repository-wide size policy, consider moving only the bitstream/XSA to Git LFS while keeping their SHA-256 values in release_manifest.json.
