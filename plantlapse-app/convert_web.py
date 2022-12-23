import os
import gzip

class Format:
    def __init__(self, load = None, save = None):
        self.data = b''
        
        if load:
            self.load(load)
            
        if save:
            self.save(save)
            
           
    def load(self, filename):
        with open(filename, 'rb') as file:
            f_in = file.read()
            
        self.data = gzip.compress(f_in)
            
        
    def c_template(self, body):
        c = "";
        # c += "#include \"include/index.hpp\"\n\n\n";
        # c += "extern \"C\" {  \n"
        # c += "const char index_html_gz[] = {  \n"
        c += "constexpr char index_html_gz[] = {  \n"
        c += ", ".join(body)
        # c += "\n};\nconst int sizeof_index_html_gz = sizeof(index_html_gz);\n"
        c += "\n};\nconstexpr int sizeof_index_html_gz = sizeof(index_html_gz);\n"
        # c += "} \n"
        
        return c
        
        
    def save(self, filename):
        hex_chars = list(map(lambda x: "0x{0:02x}".format(x),self.data))
        with open(filename, 'w') as file:
            f_in = file.write(self.c_template(hex_chars))
        
        

if __name__ == '__main__':

    b = Format("www/index.html", "main/include/index.hpp")
    