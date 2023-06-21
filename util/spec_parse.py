import xml.etree.ElementTree as et

FIX_SPEC_FILE = 'gemini_fix.xml'
HEADER_FILE = '../include/fix_spec.h'

def parse_fix_spec():
    tree = et.parse(FIX_SPEC_FILE)
    root = tree.getroot()

    fix_minor = 0
    fix_major = 0

    for f in root.iter('fix'):
            fix_major = f.get('major')
            fix_minor = f.get('minor')

    with open(HEADER_FILE, 'w') as header:
        # Write header file preamble
        header.write('#pragma once\n\n')

        header.write('#include <unordered_map>\n')
        header.write('#include <string>\n')

        header.write('\n')

        header.write(f'#define FIX_VER_MAJOR {fix_major}\n#define FIX_VER_MINOR {fix_minor}\n')

        header.write('\n')

        header.write('namespace fix_spec {\n')

        header.write('\tenum tag: int {\n')
        max_ = 0
        fields = root.find( 'fields' ).iter('field')
        for f in fields:
            field_number = f.get('number')
            field_name = f.get('name')
            if field_number != None:
                header.write(f'\t\t{field_name} = {field_number},\n')
                max_ = int(field_number)
        max_ += 1
        header.write(f'\t\tMAX_VAL = {max_}\n')
        header.write('\t};\n\n')

    
        fields = root.find('fields').iter('field')
        for f in fields:
            field_name = f.get('name')
            field_type = f.get('type')
            if field_type != 'INT' and field_type != 'CHAR' and field_type != 'STRING':
                continue

            for v in f.iter('value'):
                desc = v.get('description')
                ret = v.get('enum')
                if field_type == 'INT' or field_type == 'QTY':
                    header.write(f'\tstatic constexpr int {field_name}_{desc} = {ret};\n')
                if field_type == 'CHAR':
                    header.write(f"\tstatic const char {field_name}_{desc} = '{ret}';\n")
            
        
        header.write('};')

    print(f'fix {fix_major}:{fix_major}, output to {HEADER_FILE}')

if __name__ == '__main__':
    parse_fix_spec()