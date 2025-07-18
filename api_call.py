import google.generativeai as genai
import sys
import os
from dotenv import load_dotenv

if __name__ == '__main__':
    
    # Load environment variables from .env file
    load_dotenv()
    
    # python3 api_call.py [API_KEY] num_paragraphs query.txt
    # API_KEY is now optional - will use .env if not provided
    
    if len(sys.argv) < 3:
        print("Usage: python3 api_call.py [API_KEY] num_paragraphs query.txt")
        print("Note: API_KEY is optional if set in .env file")
        sys.exit(1)

    # Check if API key is provided as argument or use environment variable
    if len(sys.argv) >= 4:
        # API key provided as argument (backward compatibility)
        api_key = sys.argv[1]
        num_paragraphs = int(sys.argv[2])
        query_file = sys.argv[3]
    else:
        # API key from environment variable
        api_key = os.getenv('GEMINI_API_KEY')
        if not api_key:
            print("Error: No API key found!")
            print("Either:")
            print("1. Set GEMINI_API_KEY in your .env file, or")
            print("2. Provide API key as command line argument")
            sys.exit(1)
        num_paragraphs = int(sys.argv[1])
        query_file = sys.argv[2]
    
    # Configure Gemini
    genai.configure(api_key=api_key)
    
    # Initialize the model (using Gemini Flash)
    model = genai.GenerativeModel('gemini-1.5-flash')
    
    print(f"Number of paragraphs: {num_paragraphs}")

    # Read the paragraphs from the files
    paragraphs = []

    for i in range(num_paragraphs):
        filename = 'paragraph_' + str(i) + '.txt'
        print(f"Reading: {filename}")
        try:
            with open(filename, 'r', encoding='utf-8') as f:
                content = f.read().strip()
                if content:  # Only add non-empty paragraphs
                    paragraphs.append(f"Paragraph {i+1}:")
                    paragraphs.append(content)
                    paragraphs.append("")  # Empty line for separation
        except FileNotFoundError:
            print(f"Warning: {filename} not found")
            continue
    
    # Add query
    try:
        with open(query_file, 'r', encoding='utf-8') as f:
            query = f.read().strip()
            paragraphs.append("Question:")
            paragraphs.append(query)
    except FileNotFoundError:
        print(f"Error: Query file {query_file} not found")
        sys.exit(1)

    # Convert paragraphs to a single string
    content = '\n'.join(paragraphs)

    print("Sending to Gemini...")
    print("-" * 50)
    print(content)
    print("-" * 50)

    try:
        # Generate response using Gemini
        response = model.generate_content(
            content,
            generation_config=genai.types.GenerationConfig(
                max_output_tokens=1000,
                temperature=0.7,
                top_p=0.8,
                top_k=40
            )
        )

        reply = response.text
        print("\nGemini Response:")
        print("=" * 50)
        print(reply)
        print("=" * 50)
        
    except Exception as e:
        print(f"Error calling Gemini API: {e}")
        print("Please check your API key and internet connection.")
        
        # More specific error handling for common Gemini issues
        if "API_KEY_INVALID" in str(e):
            print("Your API key appears to be invalid. Please check your Gemini API key.")
        elif "QUOTA_EXCEEDED" in str(e):
            print("You have exceeded your API quota. Please check your Gemini usage limits.")
        elif "SAFETY" in str(e):
            print("Content was blocked by Gemini's safety filters.")