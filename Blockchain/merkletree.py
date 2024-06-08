import hashlib

class MerkleTree:
    def __init__(self, transactions):
        self.transactions = transactions
        self.root = self.create_merkle_tree(transactions)
        
    def create_merkle_tree(self, transactions):
        if not transactions:
            return None
        if len(transactions) == 1:
            return transactions[0]
        
        new_level = []
        for i in range(0, len(transactions), 2):
            left = transactions[i]
            right = transactions[i + 1] if i + 1 < len(transactions) else transactions[i]
            new_level.append(self.hash_pair(left, right))
        
        return self.create_merkle_tree(new_level)
    
    @staticmethod
    def hash_pair(left, right):
        return hashlib.sha256((left + right).encode('utf-8')).hexdigest()