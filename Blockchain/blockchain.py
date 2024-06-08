import hashlib
import os
from merkletree import MerkleTree
import json

class Block:
    def __init__(self, index, previous_hash, transactions, difficulty=4, is_loaded=False):
        self.index = index
        self.previous_hash = previous_hash
        self.transactions = transactions
        self.difficulty = difficulty
        self.nonce = 0
        self.merkle_root = MerkleTree([self.hash_transaction(tx) for tx in transactions]).root
        self.hash = self.mine_block() if not is_loaded else None
        if not is_loaded:
            print(f"Block mined: {self.hash}")
        
    @staticmethod
    def hash_transaction(transaction):
        return hashlib.sha256(json.dumps(transaction, sort_keys=True).encode('utf-8')).hexdigest()
    
    def mine_block(self):
        prefix_str = '0' * self.difficulty
        while True:
            block_hash = self.calculate_hash()
            if block_hash[:self.difficulty] == prefix_str:
                return block_hash
            self.nonce += 1
    
    def calculate_hash(self):
        block_string = json.dumps({
            'index': self.index,
            'previous_hash': self.previous_hash,
            'transactions': self.transactions,
            'merkle_root': self.merkle_root,
            'nonce': self.nonce
        }, sort_keys=True).encode('utf-8')
        return hashlib.sha256(block_string).hexdigest()

class Blockchain:
    def __init__(self, difficulty=4):
        self.difficulty = difficulty
        self.chain = [self.create_genesis_block()]
        self.pending_transactions = []
        self.balances = self.get_balances(0)
        self.min_balances = self.balances.copy()
        self.max_balances = self.balances.copy()
    
    def create_genesis_block(self):
        return Block(0, '0', [{'sender': 'genesis', 'recipient': 'Alice', 'amount': 100}, {'sender': 'genesis', 'recipient': 'Bob', 'amount': 50}, {'sender': 'genesis', 'recipient': 'Eva', 'amount': 70}], self.difficulty)
    
    def add_block(self, transactions):
        previous_block = self.chain[-1]
        new_block = Block(len(self.chain), previous_block.hash, transactions, self.difficulty)
        self.chain.append(new_block)
        self.update_balances(transactions)
    
    def is_chain_valid(self):
        for i in range(1, len(self.chain)):
            current_block = self.chain[i]
            previous_block = self.chain[i-1]
            if current_block.hash != current_block.calculate_hash():
                return False
            if current_block.previous_hash != previous_block.hash:
                return False
        return True
    
    def save_chain(self, filename='Blockchain/data.json'):
        with open(filename, 'w') as f:
            chain_data = [block.__dict__ for block in self.chain]
            json.dump(chain_data, f, indent=4)
    
    def load_chain(self, filename='Blockchain/data.json'):
        if os.path.exists(filename):
            with open(filename, 'r') as f:
                chain_data = json.load(f)
                self.chain = []
                for block_data in chain_data:
                    block = Block(
                        block_data['index'], 
                        block_data['previous_hash'], 
                        block_data['transactions'], 
                        block_data['difficulty'],
                        is_loaded=True
                    )
                    block.nonce = block_data['nonce']
                    block.hash = block_data['hash']
                    self.chain.append(block)
                self.balances = self.get_balances(len(chain_data))
    
    def get_balances(self, block_index):
        balances = {}
        for block in self.chain[:block_index + 1]:
            for tx in block.transactions:
                sender = tx['sender']
                recipient = tx['recipient']
                amount = tx['amount']
                
                if sender != 'genesis':
                    balances[sender] = balances.get(sender, 0) - amount
                balances[recipient] = balances.get(recipient, 0) + amount
        return balances
    
    def update_balances(self, transactions):
        for tx in transactions:
            sender = tx['sender']
            recipient = tx['recipient']
            amount = tx['amount']
            
            if sender != 'genesis':
                self.balances[sender] = self.balances.get(sender, 0) - amount
                self.min_balances[sender] = min(self.balances[sender], self.min_balances.get(sender, self.balances[sender]))
                self.max_balances[sender] = max(self.balances[sender], self.max_balances.get(sender, self.balances[sender]))
            
            self.balances[recipient] = self.balances.get(recipient, 0) + amount
            self.min_balances[recipient] = min(self.balances[recipient], self.min_balances.get(recipient, self.balances[recipient]))
            self.max_balances[recipient] = max(self.balances[recipient], self.max_balances.get(recipient, self.balances[recipient]))
    
    def min_max_balances(self, block_index):
        balances = self.get_balances(block_index)
        min_balances = {}
        max_balances = {}
        for i, block in enumerate(self.chain[:block_index + 1]):
            for tx in block.transactions:
                sender = tx['sender']
                recipient = tx['recipient']
                # amount = tx['amount']
                balances= self.get_balances(i)
                if sender != 'genesis':
                    min_balances[sender] = min(balances[sender], min_balances.get(sender, balances[sender]))
                    max_balances[sender] = max(balances[sender], max_balances.get(sender, balances[sender]))
                    
                min_balances[recipient] = min(balances[recipient], min_balances.get(recipient, balances[recipient]))
                max_balances[recipient] = max(balances[recipient], max_balances.get(recipient, balances[recipient]))
        return min_balances, max_balances
    
    def create_transaction(self, sender, recipient, amount):
        if amount <= 0:
            print("Invalid amount.")
            return False
        
        if sender == recipient:
            print("Sender and recipient cannot be the same.")
            return False
        
        sender_balance = self.balances.get(sender, 0)
        if sender_balance < amount:
            print("Sender does not have enough balance.")
            return False
        
        transaction = {
            'sender': sender,
            'recipient': recipient,
            'amount': amount
        }
        self.pending_transactions.append(transaction)
        print(f"From {sender} to {recipient}: {amount}")
        return True
    
    def mine_pending_transactions(self):
        if not self.pending_transactions:
            print("No transactions to mine.")
            return False
        self.add_block(self.pending_transactions)
        self.pending_transactions = []
        return True