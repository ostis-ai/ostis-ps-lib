# OSTIS Problem-Solver Library

[![license](https://img.shields.io/badge/License-Apache-yellow.svg)](LICENSE)

[![Docs](https://img.shields.io/badge/Docs-gray?style=for-the-badge&logo=read-the-docs)](https://ostis-ai.github.io/ostis-ps-lib)
[![Community](https://img.shields.io/badge/-Community-teal?style=for-the-badge&logo=matrix)](https://app.element.io/index.html#/room/#ostis_tech_support:matrix.org)

This repository is a centralized storage for the components of knowledge base and problem solver for systems made with the [OSTIS Technology](https://github.com/ostis-ai).

ostis-ps-lib is a collection of reusable components and libraries designed to facilitate the development of knowledge bases and problem solvers of systems in the OSTIS Technology. The repository is organized into several modules, each with its own set of functionalities and responsibilities.

It contains the following components: 

- [Library of Common Utilities](#library-of-common-utilities);
- [Non-Atomic Action Interpreter Module](#non-atomic-action-interpreter-module)
- [Dialog System Message Processing Module](#dialog-system-message-processing-module)

## Library of Common Utilities

The Library of Common Utilities is a collection of reusable utility functions, classes, and templates that can be used across various components of the OSTIS system. This library provides common functionality and reduces code duplication.

For more information about the Library of Common Utilities, [refer to the README file in the `ps-common-lib` folder](https://github.com/ostis-ai/ostis-ps-lib/tree/main/ps-common-lib).

## Non-Atomic Action Interpreter Module

The Non-Atomic Action Interpreter Module is responsible for interpreting non-atomic actions within the OSTIS.

For more information about the Non-Atomic Action Interpreter Module, [refer to the README file in the `non-atomic-action-interpreter-module` folder](https://github.com/ostis-ai/ostis-ps-lib/tree/main/non-atomic-action-interpreter-module).

## Dialog System Message Processing Module

The Dialog System Message Processing Module is designed for use in dialog systems, specifically for processing messages within the OSTIS.

## Documentation

- A brief user manual and developer docs are hosted on our [GitHub Pages](https://ostis-ai.github.io/ostis-ps-lib).
  - <details>
      <summary>Build documentation locally</summary>

    ```sh
    pip3 install mkdocs mkdocs-material
    mkdocs serve --config-file _docs-config/mkdocs.yml
    # and open http://127.0.0.1:8021/ in your browser
    ```
    </details>

## Further Reading

For more information about the ostis-ps-lib project, refer to the following resources:

- [ostis-ps-lib GitHub repository](https://github.com/ostis-ai/ostis-ps-lib);
- [ostis-ai GitHub organization](https://github.com/ostis-ai);
- [ostis-ai documentation](https://ostis-ai.github.io/);
- [sc-machine documentation](https://ostis-ai.github.io/sc-machine/).

## Feedback

Contributions to the ostis-ps-lib project are welcome! If you have any suggestions, bug reports, or feature requests, please open an issue or submit a pull request on the GitHub repository.

## License

Distributed under the MIT License. Check [LICENSE](LICENSE) for more information.
