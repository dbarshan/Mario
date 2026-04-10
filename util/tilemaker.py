from PIL import Image
import numpy as np
import argparse

def image_to_binary_matrix(image_path, block_size=16):
    img = Image.open(image_path).convert('RGB')
    pixels = np.array(img)
    
    h, w, _ = pixels.shape
    rows = h // block_size
    cols = w // block_size
    
    matrix = np.zeros((rows, cols), dtype=int)
    
    for i in range(rows):
        for j in range(cols):
            y_start = i * block_size
            x_start = j * block_size
            
            block = pixels[y_start:y_start+block_size, x_start:x_start+block_size]
            
            # Check if average color is close to magenta (255,0,255)
            avg = block.mean(axis=(0,1)).astype(int)
            if np.all(avg == [255, 0, 255]):
                matrix[i, j] = 1
    
    return matrix


def main():
    parser = argparse.ArgumentParser(description="Convert an image to a binary matrix based on magenta blocks.")
    parser.add_argument("image_path", help="Path to the input image")
    parser.add_argument("--block-size", type=int, default=16, help="Size of the blocks (default: 16)")
    parser.add_argument("--output", default="tile_map.bin", help="Output file path (default: binary_map.txt)")
    args = parser.parse_args()

    matrix = image_to_binary_matrix(args.image_path, args.block_size)
    rows, cols = matrix.shape
    
    with open(args.output, "wb") as f:
        # Write rows and cols as 4-byte little-endian integers
        f.write(np.array([rows, cols], dtype=np.int32).tobytes())
        # Write matrix data
        f.write(matrix.astype(np.uint8).tobytes())
        
    print(f"Binary matrix ({rows}x{cols}) saved to {args.output}")

if __name__ == "__main__":
    main()