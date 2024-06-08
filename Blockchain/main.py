import hashlib
import json
import os
from merkletree import MerkleTree
from blockchain import Blockchain, Block

def get_block_info(block_index):
    updated_balances = blockchain.get_balances(block_index)
    updated_min_balances, updated_max_balances = blockchain.min_max_balances(block_index)    
    updated_data = {person: {'balance': updated_balances[person], 'min': updated_min_balances[person], 'max': updated_max_balances[person]} 
                    for person in updated_balances}    
    print(f"\nBlock {block_index}: ")
    print(updated_data)


if __name__ == "__main__":
    blockchain = Blockchain()

    blockchain.create_transaction('Alice', 'Bob', 20)
    blockchain.create_transaction('Alice', 'Eva', 5)
    blockchain.create_transaction('Eva', 'Bob', 10)
    blockchain.mine_pending_transactions()

    blockchain.create_transaction('Bob', 'Eva', 30)
    blockchain.create_transaction('Bob', 'Alice', 15)
    blockchain.create_transaction('Eva', 'Alice', 15)
    blockchain.mine_pending_transactions()

    print("\nBlockchain is valid" if blockchain.is_chain_valid() else "Blockchain is invalid")

    blockchain.save_chain()
    blockchain.load_chain()

    print("Loaded blockchain is valid" if blockchain.is_chain_valid() else "Loaded blockchain is invalid")

    get_block_info(0)
    get_block_info(1)
    get_block_info(2)
