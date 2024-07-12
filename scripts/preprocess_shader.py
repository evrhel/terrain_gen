import sys
import os

def process_file(infile, included = [], first = True):
    """
    Preprocesses a shader file by resolving @include directives

    Parameters:
    - infile: The input file to process
    - included: A list of files that have already been included

    Returns:
    The processed file data, or None if an error occurred
    """

    infile = os.path.abspath(infile)
    if infile in included:
        print(f'{infile}: error: circular @include detected')
        sys.exit(1)
    
    with open(infile, 'r') as f:
        data = f.read()
        
    dir = os.path.dirname(infile)

    has_error = False

    lines = data.split('\n')

    # find @include directives
    includes = {}
    linenum = 0
    for line in lines:
        linenum += 1
        if line.startswith('@include "'):
            if not line[-1] == '"':
                print(f('{infile}:{linenum}: error: unterminated @include directive'))
                has_error = True
            else:
                name = line[10:-1]
                path = os.path.join(dir, name)

                if not os.path.exists(path):
                    print(f('{infile}:{linenum}: error: file not found: {name}'))
                    has_error = True
                else:
                    include_data = process_file(path, included=included + [infile], first=False)
                    if include_data is None:
                        has_error = True

                    includes[linenum] = (path, include_data)

    if has_error:
        return None
    
    result = ''
    
    # Version directive must be first
    if not first:
        result += f'#line 1\n'

    linenum = 0

    # create output file
    for line in lines:
        linenum += 1
        if linenum in includes:
            path, include_data = includes[linenum]

            result += f'\n// @include "{os.path.relpath(path, dir)}"\n'
            result += include_data
            result += f'#line {linenum + 1}\n'
        else:
            result += line + '\n'

    return result

def main():
    if len(sys.argv) < 3:
        print('Usage: python preprocess_shader.py <input> <output>')
        return 1

    infile = os.path.abspath(sys.argv[1])
    if not os.path.exists(infile):
        print(f'{infile}: error: file not found')
        return 1

    outfile = os.path.abspath(sys.argv[2])

    data = process_file(infile)
    if data is None:
        print(f'{infile}: error: failed to process file')
        return 1

    os.makedirs(os.path.dirname(outfile), exist_ok=True)
    with open(outfile, 'w') as f:
        f.write(data)

    print(f'{infile} -> {outfile}')
    return 0

if __name__ == '__main__':
    sys.exit(main())
