This is an agent that creates key elements order for a structure based only on types of nodes in the structure.

**Action class:**

`action_order_key_elements`

**Parameters:**

1. `structure with elements to order` - structure that contains elements that have to be ordered.

**Comment:**

* Order in which nodes are present in key elements order is as follows: NodeSuperclass, NodeClass, NodeLink, NodeRole, NodeNonRole, NodeTuple, Node.

**Workflow:**

* Agent searches for existing sc-element connected to structure via relation `nrel_key_elements_order`, if such sc-element exists then it is set as element of result structure and agent finishes its work;
* If no such sc-element is found then new sc-node is generated;
* Agent iterates through all structure sc-nodes in order of their sc-types and adds them to key sc-elements order;
* Generated key sc-elements order is connected to structure via relation `nrel_key_elements_order` and set as element of result structure.

### Example

Example of an input structure:

<img src="../images/dummy_key_elements_orderer_agent_input.png"></img>

Example of an output structure:

<img src="../images/dummy_key_elements_orderer_agent_output.png"></img>

### Agent implementation language
C++

### Result

Possible result codes:

* `sc_result_ok` - key elements order is found or generated;
* `sc_result_error` - internal error.
