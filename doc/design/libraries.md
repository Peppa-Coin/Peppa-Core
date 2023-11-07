# Libraries

| Name                     | Description |
|--------------------------|-------------|
| *libpeppacoin_cli*         | RPC client functionality used by *peppacoin-cli* executable |
| *libpeppacoin_common*      | Home for common functionality shared by different executables and libraries. Similar to *libpeppacoin_util*, but higher-level (see [Dependencies](#dependencies)). |
| *libpeppacoin_consensus*   | Stable, backwards-compatible consensus functionality used by *libpeppacoin_node* and *libpeppacoin_wallet* and also exposed as a [shared library](../shared-libraries.md). |
| *libpeppacoinconsensus*    | Shared library build of static *libpeppacoin_consensus* library |
| *libpeppacoin_kernel*      | Consensus engine and support library used for validation by *libpeppacoin_node* and also exposed as a [shared library](../shared-libraries.md). |
| *libpeppacoinqt*           | GUI functionality used by *peppacoin-qt* and *peppacoin-gui* executables |
| *libpeppacoin_ipc*         | IPC functionality used by *peppacoin-node*, *peppacoin-wallet*, *peppacoin-gui* executables to communicate when [`--enable-multiprocess`](multiprocess.md) is used. |
| *libpeppacoin_node*        | P2P and RPC server functionality used by *peppacoind* and *peppacoin-qt* executables. |
| *libpeppacoin_util*        | Home for common functionality shared by different executables and libraries. Similar to *libpeppacoin_common*, but lower-level (see [Dependencies](#dependencies)). |
| *libpeppacoin_wallet*      | Wallet functionality used by *peppacoind* and *peppacoin-wallet* executables. |
| *libpeppacoin_wallet_tool* | Lower-level wallet functionality used by *peppacoin-wallet* executable. |
| *libpeppacoin_zmq*         | [ZeroMQ](../zmq.md) functionality used by *peppacoind* and *peppacoin-qt* executables. |

## Conventions

- Most libraries are internal libraries and have APIs which are completely unstable! There are few or no restrictions on backwards compatibility or rules about external dependencies. Exceptions are *libpeppacoin_consensus* and *libpeppacoin_kernel* which have external interfaces documented at [../shared-libraries.md](../shared-libraries.md).

- Generally each library should have a corresponding source directory and namespace. Source code organization is a work in progress, so it is true that some namespaces are applied inconsistently, and if you look at [`libpeppacoin_*_SOURCES`](../../src/Makefile.am) lists you can see that many libraries pull in files from outside their source directory. But when working with libraries, it is good to follow a consistent pattern like:

  - *libpeppacoin_node* code lives in `src/node/` in the `node::` namespace
  - *libpeppacoin_wallet* code lives in `src/wallet/` in the `wallet::` namespace
  - *libpeppacoin_ipc* code lives in `src/ipc/` in the `ipc::` namespace
  - *libpeppacoin_util* code lives in `src/util/` in the `util::` namespace
  - *libpeppacoin_consensus* code lives in `src/consensus/` in the `Consensus::` namespace

## Dependencies

- Libraries should minimize what other libraries they depend on, and only reference symbols following the arrows shown in the dependency graph below:

<table><tr><td>

```mermaid

%%{ init : { "flowchart" : { "curve" : "basis" }}}%%

graph TD;

peppacoin-cli[peppacoin-cli]-->libpeppacoin_cli;

peppacoind[peppacoind]-->libpeppacoin_node;
peppacoind[peppacoind]-->libpeppacoin_wallet;

peppacoin-qt[peppacoin-qt]-->libpeppacoin_node;
peppacoin-qt[peppacoin-qt]-->libpeppacoinqt;
peppacoin-qt[peppacoin-qt]-->libpeppacoin_wallet;

peppacoin-wallet[peppacoin-wallet]-->libpeppacoin_wallet;
peppacoin-wallet[peppacoin-wallet]-->libpeppacoin_wallet_tool;

libpeppacoin_cli-->libpeppacoin_util;
libpeppacoin_cli-->libpeppacoin_common;

libpeppacoin_common-->libpeppacoin_consensus;
libpeppacoin_common-->libpeppacoin_util;

libpeppacoin_kernel-->libpeppacoin_consensus;
libpeppacoin_kernel-->libpeppacoin_util;

libpeppacoin_node-->libpeppacoin_consensus;
libpeppacoin_node-->libpeppacoin_kernel;
libpeppacoin_node-->libpeppacoin_common;
libpeppacoin_node-->libpeppacoin_util;

libpeppacoinqt-->libpeppacoin_common;
libpeppacoinqt-->libpeppacoin_util;

libpeppacoin_wallet-->libpeppacoin_common;
libpeppacoin_wallet-->libpeppacoin_util;

libpeppacoin_wallet_tool-->libpeppacoin_wallet;
libpeppacoin_wallet_tool-->libpeppacoin_util;

classDef bold stroke-width:2px, font-weight:bold, font-size: smaller;
class peppacoin-qt,peppacoind,peppacoin-cli,peppacoin-wallet bold
```
</td></tr><tr><td>

**Dependency graph**. Arrows show linker symbol dependencies. *Consensus* lib depends on nothing. *Util* lib is depended on by everything. *Kernel* lib depends only on consensus and util.

</td></tr></table>

- The graph shows what _linker symbols_ (functions and variables) from each library other libraries can call and reference directly, but it is not a call graph. For example, there is no arrow connecting *libpeppacoin_wallet* and *libpeppacoin_node* libraries, because these libraries are intended to be modular and not depend on each other's internal implementation details. But wallet code is still able to call node code indirectly through the `interfaces::Chain` abstract class in [`interfaces/chain.h`](../../src/interfaces/chain.h) and node code calls wallet code through the `interfaces::ChainClient` and `interfaces::Chain::Notifications` abstract classes in the same file. In general, defining abstract classes in [`src/interfaces/`](../../src/interfaces/) can be a convenient way of avoiding unwanted direct dependencies or circular dependencies between libraries.

- *libpeppacoin_consensus* should be a standalone dependency that any library can depend on, and it should not depend on any other libraries itself.

- *libpeppacoin_util* should also be a standalone dependency that any library can depend on, and it should not depend on other internal libraries.

- *libpeppacoin_common* should serve a similar function as *libpeppacoin_util* and be a place for miscellaneous code used by various daemon, GUI, and CLI applications and libraries to live. It should not depend on anything other than *libpeppacoin_util* and *libpeppacoin_consensus*. The boundary between _util_ and _common_ is a little fuzzy but historically _util_ has been used for more generic, lower-level things like parsing hex, and _common_ has been used for peppacoin-specific, higher-level things like parsing base58. The difference between util and common is mostly important because *libpeppacoin_kernel* is not supposed to depend on *libpeppacoin_common*, only *libpeppacoin_util*. In general, if it is ever unclear whether it is better to add code to *util* or *common*, it is probably better to add it to *common* unless it is very generically useful or useful particularly to include in the kernel.


- *libpeppacoin_kernel* should only depend on *libpeppacoin_util* and *libpeppacoin_consensus*.

- The only thing that should depend on *libpeppacoin_kernel* internally should be *libpeppacoin_node*. GUI and wallet libraries *libpeppacoinqt* and *libpeppacoin_wallet* in particular should not depend on *libpeppacoin_kernel* and the unneeded functionality it would pull in, like block validation. To the extent that GUI and wallet code need scripting and signing functionality, they should be get able it from *libpeppacoin_consensus*, *libpeppacoin_common*, and *libpeppacoin_util*, instead of *libpeppacoin_kernel*.

- GUI, node, and wallet code internal implementations should all be independent of each other, and the *libpeppacoinqt*, *libpeppacoin_node*, *libpeppacoin_wallet* libraries should never reference each other's symbols. They should only call each other through [`src/interfaces/`](`../../src/interfaces/`) abstract interfaces.

## Work in progress

- Validation code is moving from *libpeppacoin_node* to *libpeppacoin_kernel* as part of [The libpeppacoinkernel Project #24303](https://github.com/peppacoin/peppacoin/issues/24303)
- Source code organization is discussed in general in [Library source code organization #15732](https://github.com/peppacoin/peppacoin/issues/15732)
